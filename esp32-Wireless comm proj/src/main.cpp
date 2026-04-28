// main.cpp
// Libraries required for communication
#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"

// Libraries required for OLED, buzzer, and button
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <OneButton.h>

// Header files
#include "config.h"
#include "coms.h"
#include "state.h"
#include "ui.h"
#include "btn.h"
#include "power.h"
#include "beep.h"
#include "draw.h"
#include "board_info.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
    Serial.println("OLED FAIL");
    while (true);
  }

  display.clearDisplay();
  display.display();

  initButtons();

  ledcSetup(0, 1000, 8);
  ledcAttachPin(BUZZER_PIN, 0);

  setupEspNow();
  addPeer();

  Serial.println(MY_NAME);
  Serial.println(WiFi.macAddress());
}


void loop()
{
  tickButtons();
  updatePowerMode();
  updateScreen();

  if (alertReceived)
    playAlert();

  if (msgReceived) 
  {
    playBeep();
    msgReceived = false;
  }
  
  static unsigned long lastPing = 0;
  if (millis() - lastPing >= 5000)
  {
    sendPing();
    lastPing = millis();
  }
}
