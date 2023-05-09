#include "Xiaogyan.hpp"
#include "Switch.h"

// initialize
void Switch::begin()
{
    m_stateA = HIGH;
    m_stateB = HIGH;
    m_stateEnc = 0;
    
    // define Encoder's handler
    Xiaogyan.encoder.setRotatedHandler([](bool cw){
        m_stateEnc = m_stateEnc + (cw ? -1 : 1);
    });
}

// get A button event
bool Switch::getA()
{
    int state = Xiaogyan.buttonA.read();
    
    bool event = false;
    if((m_stateA == HIGH) && (state == LOW)){
        event = true;
    }
    m_stateA = state;
    
    return event;
}

// get B button event
bool Switch::getB()
{
    int state = Xiaogyan.buttonB.read();
    
    bool event = false;
    if((m_stateB == HIGH) && (state == LOW)){
        event = true;
    }
    m_stateB = state;
    
    return event;
}

// get Encoder
int Switch::getEnc()
{
    int val = m_stateEnc;
    m_stateEnc = 0;
    
    return val;
}
