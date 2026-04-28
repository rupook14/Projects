// states.h
#pragma once

#include <Adafruit_SSD1306.h>
#include "config.h"

extern Adafruit_SSD1306 display;

extern String message;

extern ScreenState currentScreen;
extern KeybMode currentKeys;
extern int menuIndex;
extern int menuScreenStart;
extern int selectedIndex;
extern bool backSelected;

extern PowerMode currentPower;
extern unsigned long lastInput;
extern int beaconInterval;

extern int alertIndex;
extern int quickIndex;

void clearAlertState();