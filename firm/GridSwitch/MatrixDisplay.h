#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "PollingTimer.h"

#define LED_BAT_POWERED     0
#define LED_BUS_POWERED     1

#define COLOR_BLACK     0
#define COLOR_RED       1 // TODO 要確認
#define COLOR_GREEN     2 // TODO 要確認
#define COLOR_ORANGE    3

// Matrix LED Display
class MatrixDisplay
{
public:
    void begin(int mode);
    void turnOff();
    void fill(int color);
    void set(int color, int number);
    void task();

private:
    int               m_mode;
    OneShotTimer      m_timer;
    bool              m_turnedOn;
};
