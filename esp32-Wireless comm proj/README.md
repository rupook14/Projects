# ESP32 Beeper
The ESP32 Beeper is a peer-to-peer encrypted messaging device that works completely offline. 
  
  Originally an academic project for a Wireless Communication and Microcontroller class, it uses ESP-NOW over IEEE 802.11 on Channel 1 (2.412 GHz) with Espressif's vendor-specific action frames (OUI 0x18FE34) for direct bidirectional communication.
Three packet types handle all communication: a 239-byte Message for text, a 9-byte AlertPacket for SOS and cancel, and a 1-byte PingPacket sent every 5 seconds to signal presence. RSSI is measured passively by capturing incoming 802.11 management frames in promiscuous mode. All messages are encrypted end‑to‑end using AES‑128‑CCMP with a shared PMK and a single LMK.
  
  *See [coms.cpp](https://github.com/francesvr/esp32-beeper/blob/main/src/coms.cpp) and [coms.h](https://github.com/francesvr/esp32-beeper/blob/main/include/coms.h) for the full communication implementation.*  
  
    
![esp32-beepers](https://github.com/user-attachments/assets/1157c72a-c1c1-4d91-896d-7b382e5d5843)
3D model of the beepers under [pcb_3dmodel](https://github.com/francesvr/esp32-beeper/blob/main/pcb_3dmodel/)

#### Origin
The two ESP32 Beeper were named after Kenji and Athena from the Filipino movie *She’s Dating the Gangster*. In the movie, the story begins when a beeper message goes to the wrong person after a pager ID gets recycled. The 1990s pagers  broadcast unencrypted messages to every device in range, and a pager only showed the message if the ID matched. Since pager IDs were reused, messages could easily end up with the wrong person.

The ESP32 Beeper is a modern version of that idea without those limitations. Instead of broadcasting, it sends messages directly between devices using the boards' MAC addresses that never change or get reassigned. All messages are AES‑128 encrypted end‑to‑end, and only the intended device can read them.
  
## Features
- Write messages to peers
- Quick-send premade messages to peers
- Send SOS alerts to peers
- View the last received message
- Check peer signal strength (RSSI) and last active time
- End-to-end AES-128 encrypted communication
- Works completely offline (no internet or router required)

## Hardware 
- Freenove ESP32-WROOM-32E Dev Kit
- 128x64 SSD1306 OLED display
- Passive buzzer
- 6x6x10 tactile push buttons
- 3.7V LiPo battery + TP4056 charging module

Circuit:
<img width="900" height="844" alt="beeper_circuit" src="https://github.com/user-attachments/assets/396c484a-e9f7-4935-8432-57ae4364e559" />

## UI Design
- The OLED UI was designed using [Lopaka](https://lopaka.app/) with a mix of custom and existing bitmap assets. *See a few screenshots under [ui_designs](https://github.com/francesvr/esp32-beeper/blob/main/ui_designs/) and bitmaps can be found under [bitmaps.cpp](https://github.com/francesvr/esp32-beeper/blob/main/src/bitmaps.cpp)*

  
## Future Work
- 3D printed enclosure
- Redesign UI

## Resources
- [ESP-NOW v4](https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/network/esp_now.html#)
- [ESP WiFi](https://docs.espressif.com/projects/esp-idf/en/v4.0.4/api-reference/network/esp_wifi.html)

## Dependencies

- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [OneButton](https://github.com/mathertel/OneButton)
- [ESP-NOW](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)


## Acknowledgements 
- [Freenove](https://freenove.com) for providing the Freenove ESP32-WROOM-32E Dev Board 3D model
