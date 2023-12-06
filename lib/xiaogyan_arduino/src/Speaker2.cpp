/*
 * Speaker2.cpp
 * Copyright (C) 2023 MATSUOKA Takashi
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "../include/Speaker2.hpp"
#include <Arduino.h>

#define USE_SP2

////////////////////////////////////////////////////////////////////////////////
// Speaker2

Speaker2* Speaker2::instance_ = nullptr;

// for nRF52840 Timer //////////////////////////////////////////////////////////
#ifdef USE_SP2
#if defined(ARDUINO_Seeed_XIAO_nRF52840) || defined(ARDUINO_Seeed_XIAO_nRF52840_Sense)

static void (*pTimerHandler)(void) = nullptr;

extern "C" void TIMER2_IRQHandler(void)
{
  if ( (NRF_TIMER2->EVENTS_COMPARE[0] != 0) &&
      ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
  {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
  }
  
  if(pTimerHandler != nullptr) pTimerHandler();
}

static void startTimer(unsigned long frequency, void (*handler)(void))
{
    const unsigned long f_TIMER = 16000000; // 16MHz @ PRESCALER = 0
    unsigned long cc = f_TIMER / frequency; // compare value
    pTimerHandler = handler;
    
    NRF_TIMER2->TASKS_STOP = 1;
    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->TASKS_CLEAR = 1;
    NRF_TIMER2->PRESCALER = 0;  // f_TIMER = 16MHz
    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit
                          << TIMER_BITMODE_BITMODE_Pos;
    NRF_TIMER2->CC[0] = cc;     // compare value

    NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Enabled
                           << TIMER_INTENSET_COMPARE0_Pos;
    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled
                         << TIMER_SHORTS_COMPARE0_CLEAR_Pos;

    NRF_TIMER2->TASKS_START = 1;  // start timer
    NVIC_EnableIRQ(TIMER2_IRQn);
}

static void stopTimer(void)
{
    NVIC_DisableIRQ(TIMER2_IRQn);
    NRF_TIMER2->TASKS_STOP = 1;  // stop timer
}

#endif  // for nRF52840 Timer //////////////////////////////////////////////////
#endif

void Speaker2::timerIsr()
{
    if (instance_->wavePtr_ != instance_->waveEnd_)
    {
#ifdef USE_SP2
#if defined(ARDUINO_XIAO_ESP32C3) || defined(ARDUINO_XIAO_ESP32S3)
        ledcWrite(instance_->ledControlChannel_, *instance_->wavePtr_++);
#elif defined(ARDUINO_Seeed_XIAO_nRF52840) || defined(ARDUINO_Seeed_XIAO_nRF52840_Sense)
        HwPWM0.writePin(instance_->speakerPin_, *instance_->wavePtr_++, false);
#endif
#endif
    }
    else
    {
#ifdef USE_SP2
#if defined(ARDUINO_XIAO_ESP32C3) || defined(ARDUINO_XIAO_ESP32S3)
        timerAlarmDisable(instance_->timer_);
#elif defined(ARDUINO_Seeed_XIAO_nRF52840) || defined(ARDUINO_Seeed_XIAO_nRF52840_Sense)
        stopTimer();
        //HwPWM0.writePin(instance_->speakerPin_, 0, false);
#endif
#endif
    }
}

Speaker2::Speaker2(int speakerPin, int speakerEnablePin, int ledControlChannel) :
    speakerPin_{ speakerPin },
    speakerEnablePin_{ speakerEnablePin },
    ledControlChannel_{ ledControlChannel },
    timer_{ nullptr },
    wavePtr_{ nullptr },
    waveEnd_{ nullptr }
{
    if (instance_ != nullptr) abort();
    instance_ = this;
}

void Speaker2::begin()
{
#ifdef USE_SP2
#if defined(ARDUINO_XIAO_ESP32C3) || defined(ARDUINO_XIAO_ESP32S3)
    ledcAttachPin(speakerPin_, ledControlChannel_);
    const uint32_t frequency = ledcSetup(ledControlChannel_, SPEAKER_PWM_FREQUENCY, SPEAKER_PWM_RESOLUTION_BITS);
    if (frequency == 0) abort();

    timer_ = timerBegin(0, getApbFrequency() / SPEAKER_TIMER_FREQUENCY, true);
    timerAttachInterrupt(timer_, timerIsr, true);
#elif defined(ARDUINO_Seeed_XIAO_nRF52840) || defined(ARDUINO_Seeed_XIAO_nRF52840_Sense)
    HwPWM0.addPin( speakerPin_ );
    HwPWM0.begin();
    HwPWM0.setResolution(SPEAKER_PWM_RESOLUTION_BITS);
    HwPWM0.setClockDiv(PWM_PRESCALER_PRESCALER_DIV_1);
#endif
#endif
}

void Speaker2::play(const uint8_t* wave, size_t length, uint32_t frequency)
{
	noInterrupts();
    wavePtr_ = wave;
    waveEnd_ = wave + length;

#ifdef USE_SP2
#if defined(ARDUINO_XIAO_ESP32C3) || defined(ARDUINO_XIAO_ESP32S3)
    timerAlarmWrite(timer_, SPEAKER_TIMER_FREQUENCY / frequency, true);
    timerAlarmEnable(timer_);
#elif defined(ARDUINO_Seeed_XIAO_nRF52840) || defined(ARDUINO_Seeed_XIAO_nRF52840_Sense)
    startTimer(frequency, timerIsr);
#endif
#endif
	interrupts();
}

bool Speaker2::busy()
{
	noInterrupts();
    bool ret = wavePtr_ != waveEnd_;
	interrupts();
	return ret;
}

void Speaker2::stop()
{
	noInterrupts();
	wavePtr_ = waveEnd_;
	interrupts();
}
////////////////////////////////////////////////////////////////////////////////
