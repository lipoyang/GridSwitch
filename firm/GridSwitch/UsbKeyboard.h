#pragma once

#include "Keyboard.h"

// USB Keyboard Class
class UsbKeyboard : public Keyboard
{
public:
    void begin();
    void wakeup();
    bool ready();
    void keyboardReport(uint8_t keycode[]);
    void keyboardRelease();
};

