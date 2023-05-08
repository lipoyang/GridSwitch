#include <Arduino.h>
#include "Switch.h"

// Time threshold
#define T1  20      // short push
#define T2  1000    // long push

// initialize
void Switch::begin(int pin)
{
    m_pin = pin;
    m_state = HIGH;
    
    pinMode(m_pin, INPUT_PULLUP);
    delay(100);
}

// get switch event
int Switch::get()
{
    int state = digitalRead(m_pin);
    
    if(m_state == HIGH){
        // HIGH -> LOW (push)
        if(state == LOW){
            m_long = false;
            m_state = LOW;
            m_t0 = millis();
        }
    }else{
        uint32_t now = millis();
        uint32_t elapsed = now - m_t0;
        
        // LOW -> HIGH (release)
        if(state == HIGH){
            m_state = HIGH;
            if(elapsed >= T2){
                // return SW_EVENT_LONG;
            }else if(elapsed >= T1){
                return SW_EVENT_SHORT;
            }
        }
        // LOW -> LOW (keep pushing)
        else{
            if(!m_long){
                if(elapsed >= T2){
                    m_long = true;
                    return SW_EVENT_LONG;
                }
            }
        }
    }
    return SW_EVENT_NONE;
}
