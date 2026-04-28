// ui.cpp
#include "ui.h"
#include "state.h"
#include "draw.h"
#include "input.h"
#include "coms.h"
#include "bitmaps.h"
#include "config.h"

const MenuItem menuItems[] =
{
    { "Write Message", SCREEN_MSG,        drawMsgScreen,       handleSendMsgInput,   { msg_icon,   15, 15 } },
    { "Send Alert",    SCREEN_SEND_ALERT, drawSendAlertScreen, handleSendAlertInput, { alert_icon, 16, 14 } },
    { "Quick Send",    SCREEN_QUICK,      drawQuickSendScreen, handleQuickSendInput, { quick_icon, 16, 16 } },
    { "View Message",  SCREEN_INBOX,      drawInboxScreen,     handleInboxInput,     { inbox_icon, 17, 16 } },
    { "RSSI",          SCREEN_RSSI,       drawRSSIScreen,      handleRSSIInput,      { rssi_icon,  15, 15 } },
};
const int menuItemCount = sizeof(menuItems) / sizeof(menuItems[0]);

const KeybModeInfo keybModes[] =
{
    { "ABC", upper_alph },
    { "abc", lower_alph },
    { "123", num_sym    }
};

void updateScreen()
{
    display.clearDisplay();

    if (alertReceived)
    {
        drawAlertReceived();
        return;
    }

    if (msgReceived)
    {
        if (millis() - msgReceivedTime < 3000)
        {
            drawMsgReceived();
            return;
        }
        else
            msgReceived = false;
    }

    if (currentScreen == SCREEN_MENU)
    {
        drawMenuLayout();
        drawMenuItems();
    }
    else
    {
        menuItems[currentScreen - 1].drawFunc();
    }
    display.display();
}