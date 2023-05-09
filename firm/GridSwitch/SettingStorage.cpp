#include <Arduino.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_TinyUSB.h>
#include "KeyMap.h"

using namespace Adafruit_LittleFS_Namespace;

// internal storage file name
#define FILENAME    "/setting.bin"

// bit mask for modifier keys
#define ALT_KEY    0x04    // Alt
#define CTRL_KEY   0x02    // Ctrl
#define SHIFT_KEY  0x01    // Shift

// modifier key code
// https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
#define HID_KEY_ALT_LEFT        0xE2
#define HID_KEY_CONTROL_RIGHT   0xE4 // Some people swap left Ctrl and CapsLock.
#define HID_KEY_SHIFT_LEFT      0xE1

// internal storage file
static File g_file(InternalFS);

// initialize
void KeyMapStorage::begin()
{
    // begin internal file system
    InternalFS.begin();
    
    // load Keymap data from Data Flash
    this->load();
}

// change Keymap Layer
void KeyMapStorage::changeLayer(bool change)
{
    int cnt = 0;
    
    while(true){
        if(change) m_layer++;
        if(m_layer >= PAGE_MAX) m_layer = 0;
        if(keyMaps[m_layer].Enabled) break;
        cnt++;
        if(cnt >= PAGE_MAX ) break;
    }
//  Serial1.print("layer = "); Serial1.println(m_layer); 
}

// set default value
void KeyMapStorage::setDefaultValue()
{
    int layer;
    
    //********** layer 0 : Visual Studio **********
    layer = 0;
    keyMaps[layer].Enabled = true;
    // Name
    memset(keyMaps[layer].Name, 0x00, 20);
    sprintf((char*)keyMaps[layer].Name, "Visual Studio");
    // Led
    keyMaps[layer].Led[0] = 93;
    keyMaps[layer].Led[1] = 0;
    keyMaps[layer].Led[2] = 200;
    // Key 0 : Start
    keyMaps[layer].Modifiers[0] = 0;
    keyMaps[layer].KeyCodes [0] = HID_KEY_F5;
    // Key 1 : Start Without Debugging
    keyMaps[layer].Modifiers[1] = CTRL_KEY;
    keyMaps[layer].KeyCodes [1] = HID_KEY_F5;
    // Key 2 : Continue
    keyMaps[layer].Modifiers[2] = 0;
    keyMaps[layer].KeyCodes [2] = HID_KEY_F5;
    // Key 3 : Break All
    keyMaps[layer].Modifiers[3] = CTRL_KEY + ALT_KEY;
    keyMaps[layer].KeyCodes [3] = HID_KEY_PAUSE; // Break key
    // Key 4 : Stop Debugging
    keyMaps[layer].Modifiers[4] = SHIFT_KEY;
    keyMaps[layer].KeyCodes [4] = HID_KEY_F5;
    // Key 5 : Step Over
    keyMaps[layer].Modifiers[5] = 0;
    keyMaps[layer].KeyCodes [5] = HID_KEY_F10;
    // Key 6 : Step Into
    keyMaps[layer].Modifiers[6] = 0;
    keyMaps[layer].KeyCodes [6] = HID_KEY_F11;
    // Key 7 : Step Out
    keyMaps[layer].Modifiers[7] = SHIFT_KEY;
    keyMaps[layer].KeyCodes [7] = HID_KEY_F11;
    // Key 8 : Run To Cursor
    keyMaps[layer].Modifiers[8] = CTRL_KEY;
    keyMaps[layer].KeyCodes [8] = HID_KEY_F10;
    // Key 9 : Restart
    keyMaps[layer].Modifiers[9] = CTRL_KEY + SHIFT_KEY;
    keyMaps[layer].KeyCodes [9] = HID_KEY_F5;

    //********** layer 1 : Eclipse **********
    layer = 1;
    keyMaps[layer].Enabled = true;
    // Name
    memset(keyMaps[layer].Name, 0x00, 20);
    sprintf((char*)keyMaps[layer].Name, "Eclipse");
    // Led
    keyMaps[layer].Led[0] = 200;
    keyMaps[layer].Led[1] = 110;
    keyMaps[layer].Led[2] = 0;
    // Key 0 : Debug
    keyMaps[layer].Modifiers[0] = 0;
    keyMaps[layer].KeyCodes [0] = HID_KEY_F11;
    // Key 1 : Run
    keyMaps[layer].Modifiers[1] = CTRL_KEY;
    keyMaps[layer].KeyCodes [1] = HID_KEY_F11;
    // Key 2 : Resume
    keyMaps[layer].Modifiers[2] = 0;
    keyMaps[layer].KeyCodes [2] = HID_KEY_F8;
    // Key 3 : Suspend
    keyMaps[layer].Modifiers[3] = 0;
    keyMaps[layer].KeyCodes [3] = 0; // not assigned
    // Key 4 : Terminate
    keyMaps[layer].Modifiers[4] = CTRL_KEY;
    keyMaps[layer].KeyCodes [4] = HID_KEY_F2;
    // Key 5 : Step Over
    keyMaps[layer].Modifiers[5] = 0;
    keyMaps[layer].KeyCodes [5] = HID_KEY_F6;
    // Key 6 : Step Into
    keyMaps[layer].Modifiers[6] = 0;
    keyMaps[layer].KeyCodes [6] = HID_KEY_F5;
    // Key 7 : Step Return
    keyMaps[layer].Modifiers[7] = 0;
    keyMaps[layer].KeyCodes [7] = HID_KEY_F7;
    // Key 8 : Run to Line
    keyMaps[layer].Modifiers[8] = CTRL_KEY;
    keyMaps[layer].KeyCodes [8] = HID_KEY_R;
    // Key 9 : Restart
    keyMaps[layer].Modifiers[9] = 0;
    keyMaps[layer].KeyCodes [9] = 0; // not assigned

    //********** layer 2 : Android Studio **********
    layer = 2;
    keyMaps[layer].Enabled = true;
    // Name
    memset(keyMaps[layer].Name, 0x00, 20);
    sprintf((char*)keyMaps[layer].Name, "Android Studio", layer);
    // Led
    keyMaps[layer].Led[0] = 0;
    keyMaps[layer].Led[1] = 200;
    keyMaps[layer].Led[2] = 40;
    // Key 0 : Debug
    keyMaps[layer].Modifiers[0] = SHIFT_KEY;
    keyMaps[layer].KeyCodes [0] = HID_KEY_F9;
    // Key 1 : Run
    keyMaps[layer].Modifiers[1] = SHIFT_KEY;
    keyMaps[layer].KeyCodes [1] = HID_KEY_F10;
    // Key 2 : Resume Program
    keyMaps[layer].Modifiers[2] = 0;
    keyMaps[layer].KeyCodes [2] = HID_KEY_F9;
    // Key 3 : Pause
    keyMaps[layer].Modifiers[3] = 0;
    keyMaps[layer].KeyCodes [3] = 0; // not assigned
    // Key 4 : Stop
    keyMaps[layer].Modifiers[4] = CTRL_KEY;
    keyMaps[layer].KeyCodes [4] = HID_KEY_F2;
    // Key 5 : Step Over
    keyMaps[layer].Modifiers[5] = 0;
    keyMaps[layer].KeyCodes [5] = HID_KEY_F8;
    // Key 6 : Step Into
    keyMaps[layer].Modifiers[6] = 0;
    keyMaps[layer].KeyCodes [6] = HID_KEY_F7;
    // Key 7 : Step Out
    keyMaps[layer].Modifiers[7] = SHIFT_KEY;
    keyMaps[layer].KeyCodes [7] = HID_KEY_F8;
    // Key 8 : Run to Cursor
    keyMaps[layer].Modifiers[8] = ALT_KEY;
    keyMaps[layer].KeyCodes [8] = HID_KEY_F9;
    // Key 9 : Rerun
    keyMaps[layer].Modifiers[9] = 0;
    keyMaps[layer].KeyCodes [9] = 0; // not assigned

    //********** layer 3 : MPLAB X **********
    layer = 3;
    keyMaps[layer].Enabled = true;
    // Name
    memset(keyMaps[layer].Name, 0x00, 20);
    sprintf((char*)keyMaps[layer].Name, "MPLAB X");
    // Led
    keyMaps[layer].Led[0] = 200;
    keyMaps[layer].Led[1] = 4;
    keyMaps[layer].Led[2] = 0;
    // Key 0 : Debug Main Project
    keyMaps[layer].Modifiers[0] = 0;
    keyMaps[layer].KeyCodes [0] = 0; // not assigned
    // Key 1 : Run Main Project
    keyMaps[layer].Modifiers[1] = 0;
    keyMaps[layer].KeyCodes [1] = 0; // not assigned
    // Key 2 : Continue
    keyMaps[layer].Modifiers[2] = 0;
    keyMaps[layer].KeyCodes [2] = HID_KEY_F5;
    // Key 3 : Pause
    keyMaps[layer].Modifiers[3] = CTRL_KEY + ALT_KEY;
    keyMaps[layer].KeyCodes [3] = HID_KEY_8;
    // Key 4 : Finish Debugger Session
    keyMaps[layer].Modifiers[4] = SHIFT_KEY;
    keyMaps[layer].KeyCodes [4] = HID_KEY_F5;
    // Key 5 : Step Over
    keyMaps[layer].Modifiers[5] = 0;
    keyMaps[layer].KeyCodes [5] = HID_KEY_F8;
    // Key 6 : Step Into
    keyMaps[layer].Modifiers[6] = 0;
    keyMaps[layer].KeyCodes [6] = HID_KEY_F7;
    // Key 7 : Step Out
    keyMaps[layer].Modifiers[7] = CTRL_KEY;
    keyMaps[layer].KeyCodes [7] = HID_KEY_F7;
    // Key 8 : Run to Cursor
    keyMaps[layer].Modifiers[8] = 0;
    keyMaps[layer].KeyCodes [8] = HID_KEY_F4;
    // Key 9 : Reset
    keyMaps[layer].Modifiers[9] = 0;
    keyMaps[layer].KeyCodes [9] = 0; // not assigned

    // layer 4-7 : not used
    for(layer = 4; layer < PAGE_MAX; layer++){
        // Enabled
        keyMaps[layer].Enabled = false;
        // Name
        memset(keyMaps[layer].Name, 0x00, 20);
        sprintf((char*)keyMaps[layer].Name, "Not Used");
        // Led
        keyMaps[layer].Led[0] = 0x00;
        keyMaps[layer].Led[1] = 0x00;
        keyMaps[layer].Led[2] = 0x00;
        // each Keys
        for(int key = 0; key < KEY_MAX; key++){
            keyMaps[layer].Modifiers[key] = 0;
            keyMaps[layer].KeyCodes [key] = HID_KEY_A + key;
        }
    }
    
    // initialize layer number
    m_layer = 0;
    
    // save to data Flash
    this->save();
}

// load Keymap data from data Flash
void KeyMapStorage::load()
{
    if (g_file.open(FILENAME, FILE_O_READ))
    {
        int len = sizeof(keyMaps);
        int readlen = g_file.read((uint8_t*)keyMaps, len);
//      int pos  = g_file.position();
//      int size = g_file.size();
        g_file.close();
        
        if(readlen == len) {
//          Serial1.println("read OK!");
//          Serial1.print("len  = "); Serial1.println(readlen);
//          Serial1.print("pos  = "); Serial1.println(pos);
//          Serial1.print("size = "); Serial1.println(size);
//          Serial1.println((char*)keyMaps[0].Name);

            // initialize layer number
            m_layer = 0;
            changeLayer(false);
        }else{
//          Serial1.println("read ERROR!");
            digitalWrite(LED_RED, LOW);
            this->setDefaultValue(); // set default value
        }
    }else{
//      Serial1.println("open(read) ERROR !");
        digitalWrite(LED_RED, LOW);
        this->setDefaultValue(); // set default value
    }
}

// save Keymap data to data Flash
void KeyMapStorage::save()
{
    if( g_file.open(FILENAME, FILE_O_WRITE) )
    {
        g_file.seek(0); // (!!!) must seek(0) to overwrite file 
        int len = sizeof(keyMaps);
        int writtenlen = g_file.write((uint8_t*)keyMaps, len);
//      int pos  = g_file.position();
//      int size = g_file.size();
        g_file.close();

        if(writtenlen == len) {
//          Serial1.println("write OK!");
//          Serial1.print("len  = "); Serial1.println(writtenlen);
//          Serial1.print("pos  = "); Serial1.println(pos);
//          Serial1.print("size = "); Serial1.println(size);
//          Serial1.println((char*)keyMaps[0].Name);
        }else{
//          Serial1.println("write ERROR!");
            digitalWrite(LED_RED, LOW);
        }
    }else{
//      Serial1.println("open(write) ERROR!");
        digitalWrite(LED_RED, LOW);
    }
}

// get LED color
uint32_t KeyMapStorage::getLedColor()
{
    uint8_t r = keyMaps[m_layer].Led[0];
    uint8_t g = keyMaps[m_layer].Led[1];
    uint8_t b = keyMaps[m_layer].Led[2];
    
    uint32_t color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
    
    return color;
}

// get Key matrix table
void KeyMapStorage::getKeyTable(uint8_t keyTable[][KEY_COMBI_MAX] )
{
    for(int i = 0; i < KEY_MAX; i++){
        // zero clear
        for(int j = 0; j < KEY_COMBI_MAX; j++){
            keyTable[i][j] = 0;
        }
        int j = 0;
        uint8_t modifiers = keyMaps[m_layer].Modifiers[i];
        // Alt
        if(modifiers & ALT_KEY) {
            keyTable[i][j] = HID_KEY_ALT_LEFT;
            j++;
        }
        // Ctrl
        if(modifiers & CTRL_KEY) {
            keyTable[i][j] = HID_KEY_CONTROL_RIGHT;
            j++;
        }
        // Shift
        if(modifiers & SHIFT_KEY) {
            keyTable[i][j] = HID_KEY_SHIFT_LEFT;
            j++;
        }
        // Key
        keyTable[i][j] = keyMaps[m_layer].KeyCodes[i];
    }
}

// execute factory reset
void KeyMapStorage::factoryReset()
{
    // Format
    InternalFS.begin();
    InternalFS.format();
}
