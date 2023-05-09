#pragma once

#include <stdbool.h>

// Switch Class
class Switch
{
public:
    void begin();           // initialize
    bool getA();            // get A button event
    bool getB();            // get B button event
    int  getEnc();          // get Encoder

private:
    int  m_stateA;          // A button state
    int  m_stateB;          // B button state
    int  m_stateEnc;        // Encoder state
};
