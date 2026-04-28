// beep.cpp
#include "beep.h"
#include "coms.h"
#include "config.h"

#include "pitches.h"

void playClick()
{
    ledcWriteTone(0, 1200);
    delay(25);
    ledcWriteTone(0, 0);
}

void playBeep()
{
    ledcWriteTone(0, 1000);
    delay(150);
    ledcWriteTone(0, 0);
}

void playSent()
{
    ledcWriteTone(0, 1400);
    delay(80);
    ledcWriteTone(0, 1800);
    delay(80);
    ledcWriteTone(0, 0);
}

void playAlert()
{
    if (!playAlertFlag) return;
    ledcWriteTone(0, 880);
    delay(300);
    ledcWriteTone(0, 0);
    delay(50);
    ledcWriteTone(0, 880);
    delay(300);
    ledcWriteTone(0, 0);
}

void playAlertSent()
{
    ledcWriteTone(0, 1400);
    delay(100);
    ledcWriteTone(0, 0);
    delay(20);
    ledcWriteTone(0, 1400);
    delay(100);
    ledcWriteTone(0, 0);
}