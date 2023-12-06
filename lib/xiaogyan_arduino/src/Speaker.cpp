/*
 * Speaker.cpp
 * Copyright (C) 2023 MATSUOKA Takashi
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "../include/Speaker.hpp"
#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////
// Speaker

Speaker::Speaker(int speakerPin, int speakerEnablePin, int ledControlChannel) :
    speakerPin_{ speakerPin },
    speakerEnablePin_{ speakerEnablePin },
    ledControlChannel_{ ledControlChannel }
{
}

void Speaker::begin()
{
    pinMode(speakerEnablePin_, OUTPUT);
    digitalWrite(speakerEnablePin_, HIGH);
#if defined(ARDUINO_XIAO_ESP32C3) || defined(ARDUINO_XIAO_ESP32S3)
    ledcAttachPin(speakerPin_, ledControlChannel_);
#elif defined(NRF52840_XXAA)
    
#endif
}

void Speaker::setPowerSupply(bool on)
{
    digitalWrite(speakerEnablePin_, on ? LOW : HIGH);
}

void Speaker::setTone(int frequency)
{
#if defined(ARDUINO_XIAO_ESP32C3) || defined(ARDUINO_XIAO_ESP32S3)
    ledcWriteTone(ledControlChannel_, frequency);
#elif defined(NRF52840_XXAA)
    if (frequency) tone(speakerPin_, frequency);
    else noTone(speakerPin_);
#endif
}

////////////////////////////////////////////////////////////////////////////////
