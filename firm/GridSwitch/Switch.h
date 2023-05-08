#pragma once

#include <stdint.h>
#include <stdbool.h>

// return value of Switch::get()
#define SW_EVENT_NONE   0 // none
#define SW_EVENT_SHORT  1 // short push
#define SW_EVENT_LONG   2 // long push

// Switch Class
class Switch
{
public:
    void begin(int pin);    // initialize
    int  get();             // get switch event

private:
    int         m_pin;      // pin number
    int         m_state;    // pin state
    uint32_t    m_t0;       // start time of push
    bool        m_long;     // long push flag
};
