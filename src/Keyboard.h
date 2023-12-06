#pragma once

#include <stdbool.h>
#include <stdint.h>

// How many codes can be reported at once
#define KEY_CODE_MAX    6

// Keyboard Base Class
class Keyboard
{
public:
    virtual void begin() = 0;
    virtual void wakeup() = 0;
    virtual bool ready() = 0;
    virtual void keyboardReport(uint8_t keycode[]) = 0;
    virtual void keyboardRelease() = 0;
};
