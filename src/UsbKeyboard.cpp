// USB Keyboard Class

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "UsbKeyboard.h"

/****************************************
    internal constants & variables
 ****************************************/

// Interval [msec]
#define INTERVAL_MS     2

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
static uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// USB HID object
static Adafruit_USBD_HID usb_hid(
    desc_hid_report,            // descriptor report
    sizeof(desc_hid_report),    // descriptor report length
    HID_ITF_PROTOCOL_KEYBOARD,  // protocol
    INTERVAL_MS,                // interval [msec]
    false                       // use out endpoint
);

/****************************************
    public API
 ****************************************/

// begin keyboard
void UsbKeyboard::begin()
{
    // USB HID begin
    usb_hid.begin();
    
    // wait until device mounted
    while( !TinyUSBDevice.mounted() ) delay(1);
}

// wake up the host
void UsbKeyboard::wakeup()
{
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    if ( TinyUSBDevice.suspended() )
    {
        TinyUSBDevice.remoteWakeup();
    }
}

// HID device ready?
bool UsbKeyboard::ready()
{
    return usb_hid.ready();
}

// keyboard report
// keycode[KEY_CODE_MAX]: pressed key codes
void UsbKeyboard::keyboardReport(uint8_t keycode[])
{
    const uint8_t report_id = 0;
    const uint8_t modifier  = 0;

    usb_hid.keyboardReport(report_id, modifier, keycode);
}

// keyboard release
void UsbKeyboard::keyboardRelease()
{
    usb_hid.keyboardRelease(0);
}
