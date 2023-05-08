#pragma once

#include <stdbool.h>
#include "PollingTimer.h"

// Power Class
class Power
{
public:
    // initialize
    void begin(int pinLedPower, int pinWakeUp);
    // LED Power ON
    void turnOnLed();
    // LED Power OFF
    void turnOffLed();
    // detect USB Vbus
    bool detectVbus(bool initial = false);
    // monitor Battery Voltage [mV]
    int getVbat();
    // go to sleep
    void sleep();
    // check Low Battery Voltage
    bool checkLowBattery();
    // check No Operation
    bool checkNoOperation();
    // kick (reset No Operation timer)
    void kick();

private:
    int  m_pinLedPower;     // LED Power ON pin
    int  m_pinWakeUp;       // Wake Up pin
    int  m_lowBatCnt;       // Low Battery Count
    bool m_usbConnected;    // USB connected? (Vbus detected?)
    IntervalTimer m_timerVbat; // Interval Timer for Vbat
    OneShotTimer  m_timerNoOp; // Timer for No Operation
};
