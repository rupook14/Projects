// input.h
#pragma once

#include "config.h"

void handleMenuInput(ButtonEvent event);
void handleSendMsgInput(ButtonEvent event);
void handleSendAlertInput(ButtonEvent event);
void handleQuickSendInput(ButtonEvent event);
void handleInboxInput(ButtonEvent event);
void handleRSSIInput(ButtonEvent event);
void handleInstrucInput(ButtonEvent event);
void handleSettingsInput(ButtonEvent event);

void sendTypedMsg();
void sendQuickMsg(int index);