// btn.cpp
// Header files
#include "btn.h"
#include "state.h"
#include "input.h"
#include "beep.h"
#include "power.h"
#include "coms.h"
#include "ui.h"
#include "config.h"

#include <OneButton.h>

OneButton btnUp   (UP_BTN_PIN,      true);
OneButton btnDown (DOWN_BTN_PIN,    true);
OneButton btnLeft (LEFT_BTN_PIN,    true);
OneButton btnRight(RIGHT_BTN_PIN,   true);
OneButton btnOK   (OK_BTN_PIN,      true);

void initButtons()
{
    pinMode(UP_BTN_PIN,     INPUT_PULLUP);
    pinMode(DOWN_BTN_PIN,   INPUT_PULLUP);
    pinMode(LEFT_BTN_PIN,   INPUT_PULLUP);
    pinMode(RIGHT_BTN_PIN,  INPUT_PULLUP);
    pinMode(OK_BTN_PIN,     INPUT_PULLUP);

    btnUp.attachClick(pressUp);
    btnUp.attachLongPressStart(holdUp);

    btnDown.attachClick(pressDown);
    // btnLeft.attachLongPressStart(holdDown);

    btnLeft.attachClick(pressLeft);
    btnLeft.attachLongPressStart(holdLeft);

    btnRight.attachClick(pressRight);
    btnRight.attachLongPressStart(holdRight);

    btnOK.attachClick(pressOk);
    btnOK.attachLongPressStart(holdOk);
}

void tickButtons()
{
    btnUp.tick();
    btnDown.tick();
    btnLeft.tick();
    btnRight.tick();
    btnOK.tick();
}

// ------------ UP ------------
void pressUp()
{
    playClick();
    lastInput = millis();
    if (currentScreen == SCREEN_MENU)
    {
        handleMenuInput(UP_PRESSED);
        return;
    }
    else 
        menuItems[currentScreen - 1].inputFunc(UP_PRESSED);
}


void holdUp()
{
    playClick();
    lastInput = millis();
    if (currentScreen == SCREEN_MENU)
        return; 
    menuItems[currentScreen - 1].inputFunc(UP_HELD);
}
 

// ------------ DOWN ------------
void pressDown()
{
    playClick();
    lastInput = millis();

    if (currentScreen == SCREEN_MENU)
    {
        handleMenuInput(DOWN_PRESSED);
        return;
    }
    else 
        menuItems[currentScreen - 1].inputFunc(DOWN_PRESSED);
}


// void holdDown()
// {

// }
 

// ------------ LEFT ------------
void pressLeft()
{
    playClick();
    wakeUp();
    if (currentScreen == SCREEN_MENU)
        return;
    menuItems[currentScreen - 1].inputFunc(LEFT_PRESSED);
}


void holdLeft()
{
    wakeUp();
    if (currentScreen == SCREEN_MENU)
        return; 
    menuItems[currentScreen - 1].inputFunc(LEFT_HELD); 
}
 

// ------------ RIGHT ------------
void pressRight()
{
    playClick();
    wakeUp();
    if (currentScreen == SCREEN_MENU)
        return; 
    menuItems[currentScreen - 1].inputFunc(RIGHT_PRESSED);
}


void holdRight()
{
    wakeUp();
    if (currentScreen == SCREEN_MENU)
        return; 
    menuItems[currentScreen - 1].inputFunc(RIGHT_HELD);
}


// ------------ OK ------------
void pressOk()
{
    playClick();
    wakeUp();

    if (alertReceived)
    {
        Serial.println("Dismissing alert");
        sendAlertCancel();
        display.clearDisplay();
        display.display();
        return;
    }

    if (currentScreen == SCREEN_MENU)
    {
        handleMenuInput(OK_PRESSED);
        return;
    }
    else
        menuItems[currentScreen - 1].inputFunc(OK_PRESSED);
}
 

void holdOk()
{
    wakeUp();
    if (currentScreen == SCREEN_MENU)
        return; 
    menuItems[currentScreen - 1].inputFunc(OK_HELD);
}