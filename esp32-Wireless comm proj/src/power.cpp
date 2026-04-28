// power.cpp
#include "power.h"
#include "state.h"
#include "esp_wifi.h"
#include <Adafruit_SSD1306.h>

void updatePowerMode()
{
    unsigned long idle = millis() - lastInput;
    if (idle < 30000)
        setPower(POWER_ACTIVE);
    else if (idle < 120000)
        setPower(POWER_IDLE);
    else
        setPower(POWER_SLEEP);
}

void wakeUp()
{
    lastInput = millis();
    if (currentPower != POWER_ACTIVE)
    {
        currentPower = POWER_ACTIVE; 
        display.ssd1306_command(SSD1306_DISPLAYON);
        display.dim(false);
        esp_wifi_set_max_tx_power(78);
        beaconInterval = 500;
    }
}

// https://docs.espressif.com/projects/esp-techpedia/en/latest/esp-friends/advanced-development/performance/modify-tx-power.html
void setPower(PowerMode mode)
{
    if (mode == currentPower) return;
    currentPower = mode;

    switch (mode)
    {
        case POWER_ACTIVE:
            display.ssd1306_command(SSD1306_DISPLAYON);
            display.dim(false);
            esp_wifi_set_max_tx_power(52);
            beaconInterval = 500;
            setCpuFrequencyMhz(80);
            break;

        case POWER_IDLE:
            display.dim(true);
            esp_wifi_set_max_tx_power(40);
            beaconInterval = 5000;
            setCpuFrequencyMhz(80);
            break;

        case POWER_SLEEP:
            display.ssd1306_command(SSD1306_DISPLAYOFF);
            esp_wifi_set_max_tx_power(34);  /// fix
            beaconInterval = 30000;
            break;
    }
}