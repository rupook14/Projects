// coms.cpp
#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"

#include "coms.h"
#include "board_info.h"
#include "state.h"
#include "beep.h"
#include "power.h"

static const char* PMK_KEY = "SharedPrime4U&1!";
static const char* LMK_KEY = "SharedL0cal4U&1!";

Message msg;
esp_now_peer_info_t peer_info;

volatile int rssi = 0;
int channel = 0;

// Per-peer signal tracking. Indexed to match mac_peers[].
volatile int peer_rssi[NUM_PEERS] = {0};
volatile unsigned long peer_last_seen[NUM_PEERS] = {0};

bool msgReceived = false;
bool alertReceived = false;
bool playAlertFlag = false;
AlertPacket lastAlert = {"", ALERT_SOS};
unsigned long msgReceivedTime  = 0;
unsigned long alertReceivedTime = 0;

// =============== Set up ESP-NOW, PMK, LMK, and promiscuous ===============
void setupEspNow()
{
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register callbacks for receiving and sending ESP-NOW data
    esp_now_register_recv_cb(esp_now_recv_cb_t(onReceived));    // calls onReceived() when data is received
    esp_now_register_send_cb(esp_now_send_cb_t(onSent));        // calls onSent() when data is sent

    // ESP-NOW uses CCMP to encrypt packets
    // Set PMK key to encrypt the LMK with AES-128
    esp_now_set_pmk((uint8_t *)PMK_KEY);

    // Enable promiscuous mode to capture all packets in the air for RSSI reading
    esp_wifi_set_promiscuous(true);     // Turn on promiscuous mode
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb); // Calls promiscuous_rx_cb() when a packet is captured
}

// =============== Add peer ===============
void addPeer()
{
    // Loop through the list of peer MAC addresses
    for (int i = 0; i < NUM_PEERS; i++)
    {
        memcpy(peer_info.peer_addr, mac_peers[i], ESP_NOW_ETH_ALEN);    // Set peer MAC address
        memcpy(peer_info.lmk, LMK_KEY, ESP_NOW_KEY_LEN);                // Set LMK key to decrypt incoming messages
        peer_info.channel = 1;                                          // Set Wi-Fi channel
        peer_info.encrypt = true;                                       // Encrypt messages with CCMP

        esp_err_t result = esp_now_add_peer(&peer_info);

        if (result == ESP_ERR_ESPNOW_FULL)
        {
            Serial.println("Max peers reached.");
            break;
        }
        else if (result != ESP_OK)
            Serial.println("Failed to add peer.");
        else
            Serial.println("Peer added successfully.");
    }
}

// =============== Sent callback ===============
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if (status != ESP_NOW_SEND_SUCCESS)
        Serial.println("Delivery failed");
}


// =============== Received callback ===============
void onReceived(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
    Serial.printf("Received %d bytes | AlertPacket: %d | Message: %d | PingPacket: %d\n",
        len, sizeof(AlertPacket), sizeof(Message), sizeof(PingPacket));

    // Check if the sender is a registered peer
    for (int i = 0; i < NUM_PEERS; i++)
    {
        if (memcmp(mac_addr, mac_peers[i], ESP_NOW_ETH_ALEN) == 0)
        {
            peer_rssi[i]      = rssi;
            peer_last_seen[i] = millis();
            break;
        }
    }

    // If ping was received, reply with pong to let peer know this board is still in range
    if (len == sizeof(PingPacket))
    {
        PingPacket pkt;
        memcpy(&pkt, incomingData, sizeof(pkt));

        if (pkt.type == 0)
        {
            uint8_t sender_mac[ESP_NOW_ETH_ALEN];
            memcpy(sender_mac, mac_addr, ESP_NOW_ETH_ALEN);
            PingPacket pong = {1};
            esp_now_send(sender_mac, (uint8_t *)&pong, sizeof(pong));
        }
        return;
    }

    // Alert packet
    if (len == sizeof(AlertPacket))
    {
        Serial.println("Alert packet received");
        AlertPacket pkt;
        memcpy(&pkt, incomingData, sizeof(pkt));

        wakeUp();
        lastAlert = pkt;
        alertReceivedTime = millis();

        if (pkt.type == ALERT_CANCEL)
        {
            clearAlertState();
            Serial.println("Alert cancelled");
        }
        else
        {
            alertReceived = true;
            playAlertFlag = true;
            Serial.println("SOS received");
        }
        return;
    }

    // Write message
    if (len == sizeof(Message))
    {
        wakeUp();
        memcpy(&msg, incomingData, sizeof(msg));
        msgReceived = true;
        msgReceivedTime = millis();
        Serial.printf("[%s]: %s\n", msg.from, msg.text);
        Serial.printf("RSSI: %d dBm\n", rssi);
    }
}

// =============== Promiscuous callback ===============
void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    // Only management frames are captured since ESP-NOW uses vendor-specific action frames
    if (type != WIFI_PKT_MGMT)
        return;

    // Action frame subtype value per IEEE 802.11 standard
    static const uint8_t ACTION_FRAME = 0xd0;

    // Get own MAC address on first call
    static uint8_t my_mac[ESP_NOW_ETH_ALEN] = {0};
    static bool get_mac  = false;
    if (!get_mac)
    {
        esp_wifi_get_mac(WIFI_IF_STA, my_mac);
        get_mac = true;
    }

    // Map the raw bytes of the 802.11 MAC header
    typedef struct __attribute__((packed))
    {
        uint8_t frame_ctrl[2];  // Frame type and subtype -- action frame subtype is 0xd0
        uint8_t duration[2];    // Duration of channel reservation
        uint8_t dst[6];         // Destination address
        uint8_t src[6];         // Source address (sender) -- randomized when encrypted
        uint8_t bcast[6];       // Always 0xFF:FF:FF:FF:FF:FF for ESP-NOW
        uint8_t seq_ctrl[2];    // Sequence number for packet ordering
    } mac_header_t;

    // Action frame body follows right after the MAC header
    typedef struct __attribute__((packed))
    {
        uint8_t padding[8]; 
        uint8_t category;           // Always 127 (0x7F) for vendor-specific action frames
        uint8_t oui[3];             // Organization Identifier -- 0x18fe34 (Espressif)
        uint8_t random_val[4];      // Prevent relay attacks
        vendor_ie_data_t ie;        // element_id, length, vendor_oui, vendor_oui_type, payload
    } action_frame_body_t;

    // Full 802.11 action frame: MAC header + action frame body
    typedef struct __attribute__((packed))
    {
        mac_header_t hdr;
        action_frame_body_t body;
    } packet_t;

    // Raw packet from the Wi-Fi chip, containing RSSI and the 802.11 frame
    const wifi_promiscuous_pkt_t *raw = (wifi_promiscuous_pkt_t *)buf;
    const packet_t *pkt = (packet_t *)raw->payload;
    const mac_header_t *hdr = &pkt->hdr;
    const action_frame_body_t *body = &pkt->body;

    // Ignore if not an action frame
    if (hdr->frame_ctrl[0] != ACTION_FRAME)
        return;

    // Ignore if not an ESP-NOW packet
    if (memcmp(body->ie.vendor_oui, ESP_OUI, 3) != 0)
        return;
    
    // Only update RSSI for frames addressed to this board and filtering out other ESP-NOW
    // devices in the room. Source address not checked since encryption randomizes it.
    if (memcmp(hdr->dst, my_mac, ESP_NOW_ETH_ALEN) != 0)
        return;

    // Get peer's RSSI and Wi-Fi Channel
    rssi    = raw->rx_ctrl.rssi;
    channel = raw->rx_ctrl.channel;
}

// =============== PING ===============
void sendPing()
{
    PingPacket ping = {0};  // type 0 = ping

    // Sends ping to each peer
    for (int i = 0; i < NUM_PEERS; i++)
        esp_now_send(mac_peers[i], (uint8_t *)&ping, sizeof(ping));
}

// =============== Send message ===============
void sendMsg(const char* text)
{
    memset(&txMsg, 0, sizeof(txMsg));
    strncpy(txMsg.from, MY_NAME, sizeof(txMsg.from) - 1);
    strncpy(txMsg.text, text, sizeof(txMsg.text) - 1);

    for (int i = 0; i < NUM_PEERS; i++)
        esp_now_send(mac_peers[i], (uint8_t *)&txMsg, sizeof(txMsg));
}

// =============== Sent Alert ===============
void sendAlert(uint8_t alertType)
{
    AlertPacket pkt;
    memset(&pkt, 0, sizeof(pkt));
    strncpy(pkt.from, MY_NAME, sizeof(pkt.from) - 1);
    pkt.type = alertType;

    for (int i = 0; i < NUM_PEERS; i++)
        esp_now_send(mac_peers[i], (uint8_t *)&pkt, sizeof(pkt));

    if (alertType == ALERT_SOS)
        playAlertSent();
}

// =============== Cancel alert ===============
void sendAlertCancel()
{
    sendAlert(ALERT_CANCEL);
    clearAlertState();
}