#include "Xiaogyan.hpp"
#include "MatrixDisplay.h"

// LED brightness
#define LED_BRIGHTNESS  2 // TODO 要調整

// font size
#define FONT_SIZE       8

// LED time limit [msec] (when battery powerd)
#define LED_TIME        3000

// initialize
void MatrixDisplay::begin(int mode)
{
    m_mode = mode;
    
    Xiaogyan.ledMatrix.setBrightness(LED_BRIGHTNESS);
    Xiaogyan.ledMatrix.fillScreen(COLOR_BLACK);
}

// turn off
void MatrixDisplay::turnOff()
{
    Xiaogyan.ledMatrix.fillScreen(COLOR_BLACK);
    
    m_turnedOn = false;
}

// fill with the color
void MatrixDisplay::fill(int color)
{
    Xiaogyan.ledMatrix.fillScreen(color);
    
    if(m_mode == LED_BUS_POWERED) return;
    m_timer.set(LED_TIME);
    m_turnedOn = true;
}

// set color and number
void MatrixDisplay::set(int color, int number);
{
    Xiaogyan.ledMatrix.drawChar(
        1, 0,               // position (x,y)  TODO 要調整
        '0' + (char)number, // number
        (uint16_t) color,   // color
        COLOR_BLACK,        // back ground color
        FONT_SIZE);         // size
    
    if(m_mode == LED_BUS_POWERED) return;
    m_timer.set(LED_TIME);
    m_turnedOn = true;
}

// LED control task
void MatrixDisplay::task()
{
    if(m_mode == LED_BUS_POWERED) return;
    
    if(!m_turnedOn) return;
    
    if(m_timer.elapsed()){
        m_turnedOn = false;
        
        this->turnOff();
    }
}
