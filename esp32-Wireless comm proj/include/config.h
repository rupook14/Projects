// config.h
#pragma once

// =================  OLED   =================
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_ADDR      0x3C

// MSG
#define MAX_MSG_LEN 241
#define LINE_WIDTH  20
#define LINES_SHOWN 2

// KEYBOARD
#define KEYB_COLS 13
#define KEYB_KEYS 26

// PEERS
#define MAX_PEERS 10

// ALERT - SOS only, no menu
#define ALERT_COUNT 1
extern const char* const alertTypes[];

// QUICK SEND
#define QUICK_MSG_COUNT 8
extern const char* const quickMessages[];

enum ScreenState
{
    SCREEN_MENU,
    SCREEN_MSG,
    SCREEN_SEND_ALERT,
    SCREEN_QUICK,
    SCREEN_INBOX,
    SCREEN_RSSI
};

enum ButtonEvent
{
    UP_PRESSED,     UP_HELD,
    DOWN_PRESSED,   DOWN_HELD,
    LEFT_PRESSED,   LEFT_HELD,
    RIGHT_PRESSED,  RIGHT_HELD,
    OK_PRESSED,     OK_HELD
};

enum KeybMode
{
    MODE_UPPER,
    MODE_LOWER,
    MODE_NUM_SYM
};

enum PowerMode { POWER_ACTIVE, POWER_IDLE, POWER_SLEEP };

typedef void (*DrawScreenFunc)();
typedef void (*InputHandlerFunc)(ButtonEvent);

struct KeybModeInfo
{
    const char *label;
    const char *chars;
};

struct Icon
{
    const unsigned char *bitmap;
    int width;
    int height;
};

struct MenuItem
{
    const char       *menu_name;
    ScreenState       screen;
    DrawScreenFunc    drawFunc;
    InputHandlerFunc  inputFunc;
    Icon              icon;
};