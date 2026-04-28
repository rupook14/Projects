// input.cpp
#include "input.h"
#include "state.h"
#include "coms.h"
#include "draw.h"
#include "beep.h"
#include "config.h"
#include "ui.h"



// ------------ MENU ------------
void handleMenuInput(ButtonEvent event)
{
    switch (event)
    {
        case UP_PRESSED:
            menuIndex--;
            if (menuIndex < 0) menuIndex = menuItemCount - 1;
            if (menuIndex < menuScreenStart) menuScreenStart = menuIndex;
            if (menuIndex == menuItemCount - 1) menuScreenStart = menuItemCount - 2;
            break;
        
        case DOWN_PRESSED:
            menuIndex++;
            if (menuIndex >= menuItemCount) 
                menuIndex = 0;
            if (menuIndex >= menuScreenStart + 2) 
                menuScreenStart = menuIndex - 1;
            if (menuIndex == 0) 
                menuScreenStart = 0;
            break;
        
        case OK_PRESSED:
            currentScreen = (ScreenState)menuItems[menuIndex].screen;
            backSelected = false;
            break;
                
        default: 
            break;
    }
}


// ------------ SEND MESSAGE ------------
void handleSendMsgInput(ButtonEvent event) 
{
    if (backSelected)
    {
        switch (event)
        {
            case DOWN_PRESSED:
                backSelected = false;
                break;
            case OK_PRESSED:
                backSelected = false;
                currentScreen = SCREEN_MENU;
                break;
            default:
                break;
        }
        return;
    }

    switch (event)
    {
        case UP_PRESSED:
            if (selectedIndex < KEYB_COLS)
                backSelected = true;
            else
                selectedIndex -= KEYB_COLS;
            break;
        
        case UP_HELD:
            currentKeys = (KeybMode)((currentKeys + 1) % 3);
            break;

        case DOWN_PRESSED:
            selectedIndex += KEYB_COLS;
            if (selectedIndex > KEYB_KEYS - 1) 
                selectedIndex -= KEYB_KEYS;
            break;
        
        // case DOWN_HELD:
        //     break;
        
        case LEFT_PRESSED:
            selectedIndex--;
            if (selectedIndex < 0) 
                selectedIndex = KEYB_KEYS - 1;
            break;

        case LEFT_HELD:
            if (message.length() > 0)
                message.remove(message.length() - 1);
            break;
        
        case RIGHT_PRESSED:
            selectedIndex++;
            if (selectedIndex > KEYB_KEYS - 1) 
                selectedIndex = 0;
            break;
        
        case RIGHT_HELD:
            if (message.length() < MAX_MSG_LEN)
                message += " ";
            break;
        
        case OK_PRESSED:
            if (message.length() < MAX_MSG_LEN)
                message += keybModes[currentKeys].chars[selectedIndex];
            break;

        case OK_HELD:
            sendTypedMsg();
            break;
        
        default:
            break;
    }

}


// ------------ SEND ALERT ------------
void handleSendAlertInput(ButtonEvent event)
{
    if (backSelected)
    {
        switch (event)
        {
            case DOWN_PRESSED:
                backSelected = false;
                break;

            case OK_PRESSED:
                backSelected = false;
                currentScreen = SCREEN_MENU;
                break;

            default:
                break;
        }
        return;
    }

    switch (event)
    {
        case UP_PRESSED:
            if (alertIndex == 0)
                backSelected = true;
            else
                alertIndex--;
            break;

        case DOWN_PRESSED:
            if (alertIndex < ALERT_COUNT - 1)
                alertIndex++;
            break;

        case OK_PRESSED:
            sendAlert(ALERT_SOS);
            currentScreen = SCREEN_MENU;
            break;

        default:
            break;
    }
}




void handleQuickSendInput(ButtonEvent event)
{
    if (backSelected)
    {
        switch (event)
        {
            case DOWN_PRESSED:
                backSelected = false;
                break;
            case OK_PRESSED:
                backSelected = false;
                currentScreen = SCREEN_MENU;
                break;
            default: break;
        }
        return;
    }

    switch (event)
    {
        case UP_PRESSED:
            if (quickIndex == 0)
                backSelected = true;
            else
                quickIndex--;
            break;

        case DOWN_PRESSED:
            if (quickIndex < QUICK_MSG_COUNT - 1)
                quickIndex++;
            break;

        case OK_PRESSED:
            sendQuickMsg(quickIndex);
            break;

        default: 
            break;
    }
}


void handleInboxInput(ButtonEvent event)      
{
    if (backSelected)
    {
        if (event == OK_PRESSED)
        {
            backSelected = false;
            currentScreen = SCREEN_MENU;
        }
        else if (event == DOWN_PRESSED)
        {
            backSelected = false;
        }
        return;
    }

    if (event == UP_PRESSED)
    {
        backSelected = true;
    }

}




void handleRSSIInput(ButtonEvent event)       
{
    if (backSelected)
    {
        switch (event)
        {
            case DOWN_PRESSED: 
                backSelected = false;                          
                break;
            case OK_PRESSED:   
                backSelected = false; 
                currentScreen = SCREEN_MENU; 
                break;
            default: 
                break;
        }
        return;
    }

    switch (event)
    {
        case UP_PRESSED:
            backSelected = true;
            break;
        default: break;
    }
}


void handleInstrucInput(ButtonEvent event)    
{

}


void handleSettingsInput(ButtonEvent event)   
{

}

void sendTypedMsg()
{
    if (message.length() == 0) return;
    sendMsg(message.c_str());
    playSent();
    drawMsgSending();
    message = "";
    currentScreen = SCREEN_MENU;
}

void sendQuickMsg(int index)
{
    sendMsg(quickMessages[index]);
    playSent();
    currentScreen = SCREEN_MENU;
}