#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "Power.h"

// PIN_VBAT     32     D32 = P0.31 (AIN7_BAT)
// VBAT_ENABLE  14     D14 = P0.14 (READ_BAT) 
#define HICHG   22  // D22 = P0.13 (BQ25100 ISET)

#define VBAT_INTERVAL   1000    // Vbat Interval [msec]
#define LOW_BATT_TIME   5       // Low Battery time [sec]
#define LOW_BATT_MV     3500    // Low Battery threshold [mV]

#define NO_OP_TIME      (10*60*1000)    // No Operation time limit [msec]

// initialize
void Power::begin(int pinLedPower, int pinWakeUp)
{
    // Debug Serial (TxD only)
    Serial1.begin(115200);
    NRF_UARTE0->PSEL.RXD = 0xFFFFFFFF; // Disable RxD
//  Serial1.println("Start!");
    
    // Power ON for NeoPixel LED
    m_pinLedPower = pinLedPower;
    pinMode(m_pinLedPower, OUTPUT);
    this->turnOnLed();
    
    // Wake Up Pin (Layer Switch)
    m_pinWakeUp = pinWakeUp;
    
    // on-board LEDs
    pinMode(LED_RED,    OUTPUT);
    pinMode(LED_GREEN,  OUTPUT);
    pinMode(LED_BLUE,   OUTPUT);
    digitalWrite(LED_RED,   HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE,  HIGH);

    // initialize to monitor Battery Voltage
    analogReference(AR_INTERNAL_2_4); // VREF = 2.4V
    analogReadResolution(10);         // 10bit A/D
    pinMode(VBAT_ENABLE, OUTPUT);     // READ_BAT pin = LOW
    digitalWrite(VBAT_ENABLE, LOW);
    
    // begin Interval Timer
    m_timerVbat.set(VBAT_INTERVAL);
    
    // begin No Operation Timer
    m_timerNoOp.set(NO_OP_TIME);
    
    // detect Vbus
    delay(100);
    this->detectVbus(true);
}

// LED Power ON
void Power::turnOnLed()
{
//  pinMode(m_pinLedPower, OUTPUT);
    digitalWrite(m_pinLedPower, HIGH);
}

// LED Power OFF
void Power::turnOffLed()
{
//  pinMode(m_pinLedPower, INPUT);  
    digitalWrite(m_pinLedPower, LOW);
}

// detect USB Vbus
// initial: first time?
bool Power::detectVbus(bool initial)
{
    bool connected;
    
    // detect Vbus
    uint32_t usb_reg = NRF_POWER->USBREGSTATUS;
    if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk){
        connected = true;
    }else{
        connected = false;
    }
    
    if(initial == false){
        // connect or disconnect -> reset
        if(connected != m_usbConnected){
//          Serial1.println("Reset!");
//          delay(1000);
            NVIC_SystemReset();
        }
    }else{
        m_usbConnected = connected;
    }
    return m_usbConnected;
}

// monitor Battery Voltage [mV]
int Power::getVbat()
{
    int vbat_raw = analogRead(PIN_VBAT);
    int vbat_mv = vbat_raw * 2400 / 1023; // VREF = 2.4V, 10bit A/D
    vbat_mv = vbat_mv * 1510 / 510;       // 1M + 510k / 510k
    
    return vbat_mv;
}

// go to sleep
void Power::sleep()
{
    // Battery powered
    if(!this->detectVbus())
    {
//      Serial1.println("Sleep!");
//      delay(1000);
        
        // power off for NeoPixel LED
        this->turnOffLed();
        
        // setup wake-up pin.
        pinMode(m_pinWakeUp,  INPUT_PULLUP_SENSE);
        // power down nrf52.
        sd_power_system_off(); 
    }
}

// check Battery Voltage
bool Power::checkLowBattery()
{
    // Vbat interval
    if(!m_timerVbat.elapsed()) return false;
    
    int vbat = this->getVbat();
//  Serial1.print("Vbat = "); Serial1.println(vbat);

    // Battery powered
    if(!this->detectVbus())
    {
        if(vbat < LOW_BATT_MV){
            m_lowBatCnt++;
            if(m_lowBatCnt >= LOW_BATT_TIME){
//              Serial1.println("Low Battery!");
                return true;
            }
        }else{
            m_lowBatCnt = 0;
        }
    }
    return false;
}

// check No Operation
bool Power::checkNoOperation()
{
    // Battery powered
    if(!this->detectVbus())
    {
        if(m_timerNoOp.elapsed()){
//          Serial1.println("No Operation!");
            return true;
        }
    }
    return false;
}

// kick (reset No Operation timer)
void Power::kick()
{
    // reset No Operation Timer
    m_timerNoOp.set(NO_OP_TIME);
}
