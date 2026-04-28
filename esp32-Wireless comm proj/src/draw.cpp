// draw.cpp
#include "draw.h"
#include "state.h"
#include "coms.h"
#include "bitmaps.h"
#include "ui.h"
#include "config.h"

#include "board_info.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Picopixel.h"
#include "FreeSans12pt7b.h"

// Keyboard modes
// Hold UP button to switch keys
const char upper_alph[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char lower_alph[] = "abcdefghijklmnopqrstuvwxyz";
const char num_sym[]    = "0123456789!@#$%^&*()?.,/:;";


float chToFreq(int channel)
{
    return 2.407f + (channel * 0.005f);
}

// ================= MENU =================
void drawMenuLayout()
{
    display.drawBitmap(0, 0, menu_borders, 128, 64, 1);
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&Picopixel);
    display.setCursor(4, 7);
    display.print("MAIN MENU");
    display.drawBitmap(122, 13, menu_scroll_bar, 3, 48, 1);
}

void drawMenuItems()
{
    drawScrollBarCursor(menuItemCount, menuIndex, 123, 18, 58);

    for (int i = 0; i < 2; i++)
    {
        int item = menuScreenStart + i;
        if (item >= menuItemCount) break;

        int icon_y    = (i == 0) ? 17 : 40;
        int text_y    = (i == 0) ? 20 : 44;
        int icon_colour;

        if (item == menuIndex)
        {
            display.drawBitmap(4, icon_y - 3, menu_select_cursor, 116, 22, 1);
            display.setTextColor(0);
            icon_colour = 0;
        }
        else
        {
            display.setTextColor(1);
            icon_colour = 1;
        }

        display.drawBitmap(10, icon_y,
            menuItems[item].icon.bitmap,
            menuItems[item].icon.width,
            menuItems[item].icon.height,
            icon_colour);
        display.setFont();
        display.setCursor(32, text_y);
        display.print(menuItems[item].menu_name);
    }
}

void drawScrollBarCursor(int itemCount, int index, int scroll_cursor_x, int scroll_bar_start, int scroll_bar_end)
{
    int range          = scroll_bar_end - scroll_bar_start;
    int scroll_cursor_h = range / itemCount;
    int scroll_cursor_y = scroll_bar_start + (scroll_cursor_h * index);

    if (scroll_cursor_y + scroll_cursor_h > scroll_bar_end)
        scroll_cursor_y = scroll_bar_end - scroll_cursor_h;

    display.drawLine(scroll_cursor_x, scroll_cursor_y,
                     scroll_cursor_x, scroll_cursor_y + scroll_cursor_h, 1);
}


// ================= BACK BUTTON =================
void drawBackButton()
{
    if (backSelected)
        display.drawBitmap(0, 0, back_btn, 12, 11, 0, 1);
    else
        display.drawBitmap(0, 0, back_btn, 12, 11, 1, 0);
    display.drawBitmap(0, 0, back_btn_border, 13, 11, 0);
}


// ================= MESSAGE SCREENS =================
void drawMsgReceived()
{
    display.clearDisplay();
    display.setFont();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(18, 2);
    display.print("New Message!");
    display.setCursor(0, 18);
    display.print("From: ");
    display.print(msg.from);
    display.setCursor(0, 34);
    display.print(msg.text);
    display.display();
}

void drawMsgSending()
{
    display.clearDisplay();
    display.setFont();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(24, 10);
    display.print("Sending...");
    display.display();
    delay(800);
}

void drawMsgScreen()
{
    drawBackButton();
    display.drawBitmap(0, 0, msg_screen_borders, 128, 64, 1);

    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&Picopixel);

    display.setCursor(21, 6);
    display.print("To: ");
    if ((millis() - peer_last_seen[0]) < 30000)
        display.print(peer_names[0]);
    else
        display.print("NONE");

    display.setCursor(85, 6);
    display.print(keybModes[currentKeys].label);

    display.setCursor(99, 6);
    display.print(String(message.length()) + "/" + String(MAX_MSG_LEN));

    display.setFont();
    display.setTextColor(0);
    drawMsgTyped();
    drawKeyboard();
    display.setTextColor(1);
    display.display();
}

void drawMsgTyped()
{
    int totalChars = message.length();
    int totalLines = (totalChars / LINE_WIDTH) + 1;
    int startLine  = max(0, totalLines - LINES_SHOWN);

    for (int i = 0; i < LINES_SHOWN; i++)
    {
        int lineIndex = startLine + i;
        int charStart = lineIndex * LINE_WIDTH;
        if (charStart >= totalChars) break;

        String line = message.substring(charStart, min(charStart + LINE_WIDTH, totalChars));
        display.setCursor(4, 14 + (i * 11));
        display.print(line);
    }
}

void drawKeyboard()
{
    const int startX  = 7;
    const int row1Y   = 42;
    const int row2Y   = 53;
    const int spacing = 9;

    for (int i = 0; i < 26; i++)
    {
        int x = (i < 13) ? startX + i * spacing : startX + (i - 13) * spacing;
        int y = (i < 13) ? row1Y : row2Y;

        if (i == selectedIndex && !backSelected)
        {
            display.fillRect(x - 1, y - 1, 7, 9, 1);
            display.setTextColor(0);
        }
        else
            display.setTextColor(1);

        display.setCursor(x, y);
        display.print(keybModes[currentKeys].chars[i]);
    }
}


// ================= INBOX =================
void drawInboxScreen()
{
    drawBackButton();

    display.setFont(&Picopixel);
    display.setTextColor(1);
    display.setCursor(45, 8);
    display.print("INBOX");
    display.setFont();

    if (msg.text[0] == '\0')
    {
        display.setCursor(24, 32);
        display.print("No messages");
    }
    else
    {
        display.setCursor(4, 18);
        display.print("From: ");
        display.print(msg.from);
        display.setCursor(4, 30);
        display.print(msg.text);
    }

    display.display();
}


// ================= ALERT =================
void drawSendAlertScreen()
{
    display.setFont(&FreeSans12pt7b);
    
    display.fillRect(0, 0, 128, 64, 1);

    display.setTextColor(0);
    display.setCursor(39, 31);
    display.print("SOS");

    display.setFont();
    display.setCursor(14, 39);
    display.print("Sent to all peers");

    display.setCursor(7, 51);
    display.print("Press OK to dismiss");

    display.display();
}

void drawAlertReceived()
{
    display.clearDisplay();
    
    display.fillRect(0, 0, 128, 64, 1);

    display.setFont(&FreeSans12pt7b);
    display.setTextColor(0);
    display.setCursor(39, 39);
    display.print("SOS");

    display.setFont();
    display.setTextSize(1);

    display.setCursor(3, 5);
    display.print("From: ");
    display.print(lastAlert.from);

    display.setCursor(11, 53);
    display.print("PRESS OK TO ACCEPT");

    display.display();
}

// ================= QUICK SEND =================
void drawQuickSendScreen()
{
    drawBackButton();

    display.setFont(&Picopixel);
    display.setTextColor(1);
    display.setCursor(40, 8);
    display.print("QUICK SEND");
    display.setFont();

    int startItem = max(0, quickIndex - 3);

    for (int i = 0; i < 4; i++)
    {
        int item = startItem + i;
        if (item >= QUICK_MSG_COUNT) break;

        int y = 16 + (i * 12);

        if (item == quickIndex && !backSelected)
        {
            display.fillRect(4, y - 2, 120, 11, 1);
            display.setTextColor(0);
        }
        else
            display.setTextColor(1);

        display.setCursor(8, y);
        display.print(quickMessages[item]);
    }

    display.setTextColor(1);
    display.display();
}


// ================= RSSI =================
void drawRSSIScreen()
{
    drawBackButton();
    display.setTextColor(1);

    display.setFont(&Picopixel);

    // Channel and frequency
    display.setCursor(34, 4);
    display.print("Channel ");
    display.print(channel);
    display.setCursor(77, 4);
    display.print(chToFreq(channel), 3);
    display.print(" GHz");

    display.setCursor(2, 26);
    display.print("Peer:");
    display.setCursor(2, 50);
    display.print("Last seen:");

    display.setFont();

    display.setCursor(2, 31);
    display.print(peer_names[0]);

    // Last seen
    display.setCursor(2, 55);
    unsigned long ago = (millis() - peer_last_seen[0]) / 1000;
    display.print(ago);
    display.print("s ago");

    // RSSI value
    display.setCursor(58, 16);
    display.print(peer_rssi[0]);
    display.print(" dBm");

    // Signal bars
    int displayRSSI = peer_rssi[0];
    int signal = 0;
    if ((millis() - peer_last_seen[0]) < 15000)
    {
        if      (displayRSSI > -55) signal = 4;
        else if (displayRSSI > -67) signal = 3;
        else if (displayRSSI > -78) signal = 2;
        else if (displayRSSI > -90) signal = 1;
        else                        signal = 0;
    }

    int barWidth = 15;
    int barGap   = 3;
    int baseY    = 64;
    int startX   = 58;

    for (int i = 0; i < 4; i++)
    {
        int barHeight = (i + 1) * 12;
        int x = startX + i * (barWidth + barGap);
        int y = baseY - barHeight;

        if (i < signal)
            display.fillRect(x, y, barWidth, barHeight, 1);
        else
            display.drawRect(x, y, barWidth, barHeight, 1);
    }

    display.display();
}