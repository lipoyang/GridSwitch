#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Adafruit_NeoPixel.h>
#include "PollingTimer.h"

#define LED_BAT_POWERED     0
#define LED_BUS_POWERED     1

#define COLOR_RED       0xFF0000
#define COLOR_GREEN     0x00FF00
#define COLOR_BLUE      0x0000FF
#define COLOR_YELLOW    0xFFFF00
#define COLOR_CYAN      0x00FFFF
#define COLOR_MAGENTA   0xFF00FF
#define COLOR_WHITE     0xFFFFFF
#define COLOR_BLACK     0x000000

// Color LED Class
class ColorLed
{
public:
    ColorLed(int pin);
    void begin(int mode);
    void setColor(uint32_t color);
    void turnOff();
    void turnOnWhile();
    void turnOnMoment();
    void endTempOn();
    void task();

private:
    int               m_mode;
    Adafruit_NeoPixel m_neoPixel;
    uint32_t          m_color;
    OneShotTimer      m_timer;
    bool              m_turnedOn;
};
