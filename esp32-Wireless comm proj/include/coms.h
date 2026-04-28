// coms.h
#pragma once

#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "board_info.h"

// Espressif Organization Identifier -- used to identify ESP-NOW packets
static const uint8_t ESP_OUI[] = {0x18, 0xfe, 0x34};

// The maximum payload size for a single ESP-NOW transmission is 250 bytes.
typedef struct Message
{                   // The struct totals 239 bytes, 11 bytes under the limit.
    char from[8];   // Sender's name -- up to 7 characters (null terminated)
    char text[230]; // Text message -- up to 229 characters (null terminated)
    bool isSOS;     // Whether the message is an SOS alert -- 1 byte
} Message;

typedef struct
{
    uint8_t type; // 0 = ping, 1 = pong (reply)
} PingPacket;

enum AlertType : uint8_t
{
    ALERT_SOS    = 0,
    ALERT_CANCEL = 1
};

typedef struct
{
    char    from[8];
    uint8_t type;
} AlertPacket;

extern Message txMsg;
extern Message msg;
extern esp_now_peer_info_t peer_info;

extern volatile int rssi;
extern int channel;

// Per-peer signal tracking
extern volatile int peer_rssi[NUM_PEERS]; // Last RSSI reading per peer
extern volatile unsigned long peer_last_seen[NUM_PEERS]; // millis() of last ping from peer

extern bool          msgReceived;
extern bool          alertReceived;
extern bool          playAlertFlag;
extern AlertPacket   lastAlert;
extern unsigned long msgReceivedTime;
extern unsigned long alertReceivedTime;

void setupEspNow();
void addPeer();
void sendPing();
void sendMsg(const char* text);
void sendAlert(uint8_t alertType);
void sendAlertCancel();

// Callbacks
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void onReceived(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type);