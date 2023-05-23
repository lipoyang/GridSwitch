#include <Adafruit_TinyUSB.h>
#include "Xiaogyan.hpp"
#include "UsbKeyboard.h"
#include "BleKeyboard.h"
#include "SerialCommand.h"
#include "SettingStorage.h"
#include "Power.h"
#include "Switch.h"
#include "MatrixDisplay.h"
#include "PollingTimer.h"

#define PIN_A            9  // Button A Pin
#define PIN_B            7  // Button B Pin

#define KEY_INTERVAL     5  // Key Interval [msec]
#define HID_KEY_DELAY    4  // HID Key event delay [msec]

// Buttons and Encoder
Switch gridSwitch;
// Matrix LED Display
MatrixDisplay matrixDisplay;
// Power Controller
Power power;

// USB Keyboard
UsbKeyboard usbKeyboard;
// BLE Keyboard
BleKeyboard bleKeyboard;
// USB or BLE Keyboard
Keyboard *keyboard;

// Serial Command
SerialCommand serialCommand;
// Setting Storage
SettingStorage settingStorage;

// is USB connected?
bool isUsbConnected;

// Interval Timer
IntervalTimer interval;

// Initialize
void setup()
{
    // XIAOGYAN
    Xiaogyan.begin();
    
    // Power
    power.begin(PIN_A, PIN_B);
    
    // Buttons and Encoder
    gridSwitch.begin();
    
    // Matrix LED Display
    if (power.detectVbus()) {
        matrixDisplay.begin(LED_BUS_POWERED);
    } else {
        matrixDisplay.begin(LED_BAT_POWERED);
    }
    
    // detect VBUS
    if (power.detectVbus()) {
        // USB Keyboard
        digitalWrite(LED_GREEN, LOW);
        keyboard = &usbKeyboard;
        isUsbConnected = true;
        
        // Factory Reset?
        if( (Xiaogyan.buttonA.read() == LOW) &&
            (Xiaogyan.buttonB.read() == LOW) )
        {
            matrixDisplay.fill(COLOR_RED);
            delay(1000);
            settingStorage.factoryReset();
            matrixDisplay.turnOff();
            delay(3000);
            NVIC_SystemReset();
        }
        
    } else {
        // BLE Keyboard
        digitalWrite(LED_BLUE,  LOW);
        keyboard = &bleKeyboard;
        isUsbConnected = false;
    }

    // begin HID (USB of BLE)
    keyboard->begin();
    
    // begin Serial Command
    serialCommand.commandTable = settingStorage.commandTable;
    if(isUsbConnected) serialCommand.begin();
    
    // Setting Storage
    settingStorage.begin();
    
    // init Matrix LED Display
    matrixDisplay.turnOff();
    
    // begin Interval Timer
    interval.set(KEY_INTERVAL);
}

// Main Loop
void loop()
{
    // XIAOGYAN
    Xiaogyan.doWork();
    
    // key interval
    if(!interval.elapsed()) return;

    // Matrix LED Display control
    matrixDisplay.task();
    
    // check low battery voltage
    bool lowBattery = power.checkLowBattery();
    // check no operation
    bool noOperation = power.checkNoOperation();
    // sleep?
    if(lowBattery || noOperation){
        matrixDisplay.turnOff();
        power.sleep();
    }
    
    // serial command task
    if(isUsbConnected){
        int ret = serialCommand.task();
        if(ret == RET_WRITE){
            settingStorage.save();
            matrixDisplay.turnOff();
        }
    }
    
    // skip if hid is not ready
    if ( !keyboard->ready() ) return;

    // get Buttons and Encoder
    if(gridSwitch.getA()) settingStorage.setA();
    if(gridSwitch.getB()) settingStorage.setB();
    int enc = gridSwitch.getEnc();
    if(enc != 0) settingStorage.setEnc(enc);
    
    // event
    if(settingStorage.todo()){
        // wake up
        keyboard->wakeup();
        
        // get command
        int len = settingStorage.getCommandLength();
        uint8_t *command = settingStorage.getCommandCode();
        
        // send command
        for(int i = 0; i <= len;  /* i++ */ ){
            uint8_t keycode[KEY_CODE_MAX] = { 0 };
            for(int j = 0; j < KEY_CODE_MAX; j++){
                keycode[j] = command[i];
                i++;
                if(i > len) break;
            }
            // key report
            keyboard->keyboardReport(keycode);
            delay(HID_KEY_DELAY);
            // key release
            keyboard->keyboardRelease();
            delay(HID_KEY_DELAY);
        }
        // display
        int color = (settingStorage.getUnit() == UNIT_A) ? COLOR_GREEN : COLOR_RED;
        int number = settingStorage.getNumber();
        matrixDisplay.set(color, number);
        
        power.kick();
    }
}
