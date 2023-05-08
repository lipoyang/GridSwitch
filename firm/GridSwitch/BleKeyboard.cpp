// BLE Keyboard Class

#include <Arduino.h>
#include <bluefruit.h>
#include "BleKeyboard.h"

/****************************************
    internal constants & variables
 ****************************************/

// TX power [dBm]
// Supported tx_power value ( Check bluefruit.h )
// - nRF52832: -40, -20, -16, -12, -8, -4, 0, +3, +4
// - nRF52840: -40, -20, -16, -12, -8, -4, 0, +2, +3, +4, +5, +6, +7, +8
#define TX_POWER    4

// BLE Device Information Service
static BLEDis bledis;

// BLE HID
static BLEHidAdafruit blehid;

/****************************************
    public API
 ****************************************/

// begin keyboard
void BleKeyboard::begin()
{
    Bluefruit.begin();
    Bluefruit.setTxPower(TX_POWER);

    // Configure and Start Device Information Service
    bledis.setManufacturer("Nara Kikaiken"); // TODO
    bledis.setModel("Debugger Keyboard");    // TODO
    bledis.begin();

    // Start BLE HID
    /* Note: 
     *   Apple requires min connection interval >= 20ms
     *   However for HID and MIDI, Apple accepts 11.25ms interval.
     *   BLEHidAdafruit::begin() tries to set 11.25ms(min) - 15ms(max).
     */
    blehid.begin();

    // Set connection interval (min, max)
    /* Note:
     *   already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
     *   min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
     */
    // Bluefruit.Periph.setConnInterval(9, 12);

    // set up and start advertising
    startAdvertise();
}

// wake up the central
void BleKeyboard::wakeup()
{
    // TODO
}

// HID device ready?
bool BleKeyboard::ready()
{
    return true; // TODO
}

// keyboard report
// keycode[KEY_CODE_MAX]: pressed key codes
void BleKeyboard::keyboardReport(uint8_t keycode[])
{
    const uint8_t modifier  = 0;
    
    blehid.keyboardReport(modifier, keycode);
}

// keyboard release
void BleKeyboard::keyboardRelease()
{
    blehid.keyRelease();
}

/****************************************
    private functions
 ****************************************/

// set up and start advertising
void BleKeyboard::startAdvertise(void)
{  
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
    // Include BLE HID service
    Bluefruit.Advertising.addService(blehid);

    // There is enough room for the dev name in the advertising packet
    Bluefruit.Advertising.addName();

    /* Start Advertising
     * - Enable auto advertising if disconnected
     * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     * - Timeout for fast mode is 30 seconds
     * - Start(timeout) with timeout = 0 will advertise forever (until connected)
     * 
     * For recommended advertising interval
     * https://developer.apple.com/library/content/qa/qa1931/_index.html   
     */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
    Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}
