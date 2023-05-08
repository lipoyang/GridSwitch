#include "ColorLed.h"

// LED brightness
#define LED_BRIGHTNESS  16

// LED time limit [msec] (when battery powerd)
#define LED_TIME1       3000 // for a while
#define LED_TIME2       300  // for a moment

// constructor
ColorLed::ColorLed(int pin) : m_neoPixel(1, pin, NEO_GRB + NEO_KHZ800)
{
    m_turnedOn = false;
}

// initialize
void ColorLed::begin(int mode)
{
    m_mode = mode;
    
    // Layer LED
    m_neoPixel.begin();
    m_neoPixel.setBrightness(LED_BRIGHTNESS);
}

// set color of LED and turn on
void ColorLed::setColor(uint32_t color)
{
    m_color = color;
    
    m_neoPixel.setPixelColor(0, m_color);
    m_neoPixel.show();
    
    if(m_mode == LED_BUS_POWERED) return;
    m_timer.set(LED_TIME1);
    m_turnedOn = true;
}

// turn off LED
void ColorLed::turnOff()
{
    m_neoPixel.setPixelColor(0, 0);
    m_neoPixel.show();
    
    m_turnedOn = false;
}

// turn on LED for a while
void ColorLed::turnOnWhile()
{
    if(m_mode == LED_BUS_POWERED) return;
    
    m_neoPixel.setPixelColor(0, m_color);
    m_neoPixel.show();
    
    m_timer.set(LED_TIME1);
    m_turnedOn = true;
}

// turn on LED for a moment
void ColorLed::turnOnMoment()
{
    if(m_mode == LED_BUS_POWERED) return;
    
    m_neoPixel.setPixelColor(0, m_color);
    m_neoPixel.show();
    
    m_timer.set(LED_TIME2);
    m_turnedOn = true;
}

// LED control task
void ColorLed::task()
{
    if(m_mode == LED_BUS_POWERED) return;
    
    if(!m_turnedOn) return;
    
    if(m_timer.elapsed()){
        m_turnedOn = false;
        m_neoPixel.setPixelColor(0, m_neoPixel.Color(0,0,0));
        m_neoPixel.show();
    }
}
