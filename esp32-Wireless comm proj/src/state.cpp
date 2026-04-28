// state.cpp

#include <Arduino.h>

#include "state.h"
#include "coms.h"
#include "config.h"

Message txMsg = {"", ""};
String message = "";

ScreenState currentScreen = SCREEN_MENU;
KeybMode currentKeys = MODE_UPPER;
int menuIndex = 0;
int menuScreenStart = 0;
int selectedIndex = 0;
bool backSelected = false;

PowerMode currentPower = POWER_ACTIVE;
unsigned long lastInput = 0;
int beaconInterval = 500;

int alertIndex = 0;
int quickIndex = 0;

void clearAlertState()
{
    alertReceived = false;
    playAlertFlag = false;
    noTone(BUZZER_PIN);
}