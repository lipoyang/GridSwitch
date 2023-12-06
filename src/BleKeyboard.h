#pragma once

#include "Keyboard.h"

// BLE Keyboard Class
class BleKeyboard : public Keyboard
{
public:
    void begin();
    void wakeup();
    bool ready();
    void keyboardReport(uint8_t keycode[]);
    void keyboardRelease();

private:
    void startAdvertise();
};

