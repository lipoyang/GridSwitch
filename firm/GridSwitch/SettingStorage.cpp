#include <string.h>
#include <Arduino.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_TinyUSB.h>
#include "SettingStorage.h"

using namespace Adafruit_LittleFS_Namespace;

// internal storage file name
#define FILENAME    "/setting.bin"

// internal storage file
static File g_file(InternalFS);

// initialize
void SettingStorage::begin()
{
    m_todo = false;
    m_unit = 0;
    m_number = -1;
    
    // begin internal file system
    InternalFS.begin();
    
    // load data from Data Flash
    this->load();
}

// set default value
void SettingStorage::setDefaultValue()
{
    m_todo = false;
    m_unit = 0;
    m_number = -1;
    
    static const char DEF_VALUE[COMMAND_MAX][COMMAND_LEN] = {
        "grid mm 1.27 2",       // A 1
        "grid mm 0.625  2",     // A 2
        "grid mm 1 5",          // A 3
        "grid mm 0.5 2",        // A 4
        "",                     // A 5
        "grid inch 0.05 2",     // B 1
        "grid inch 0.025 2",    // B 2
        "",                     // B 3
        "",                     // B 4
        ""                      // B 5
    };
    
    memcpy(commandTable, DEF_VALUE, sizeof(commandTable));
    
    // save to data Flash
    this->save();
}

// load Keymap data from data Flash
void SettingStorage::load()
{
    if (g_file.open(FILENAME, FILE_O_READ))
    {
        int len = sizeof(commandTable);
        int readlen = g_file.read((uint8_t*)commandTable, len);
//      int pos  = g_file.position();
//      int size = g_file.size();
        g_file.close();
        
        if(readlen == len) {
//          Serial1.println("read OK!");
//          Serial1.print("len  = "); Serial1.println(readlen);
//          Serial1.print("pos  = "); Serial1.println(pos);
//          Serial1.print("size = "); Serial1.println(size);
//          Serial1.println((char*)commandTable[0]);
            m_todo = false;
            m_unit = 0;
            m_number = -1;
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
        int len = sizeof(commandTable);
        int writtenlen = g_file.write((uint8_t*)commandTable, len);
//      int pos  = g_file.position();
//      int size = g_file.size();
        g_file.close();

        if(writtenlen == len) {
//          Serial1.println("write OK!");
//          Serial1.print("len  = "); Serial1.println(writtenlen);
//          Serial1.print("pos  = "); Serial1.println(pos);
//          Serial1.print("size = "); Serial1.println(size);
//          Serial1.println((char*)commandTable[0]);
        }else{
//          Serial1.println("write ERROR!");
            digitalWrite(LED_RED, LOW);
        }
    }else{
//      Serial1.println("open(write) ERROR!");
        digitalWrite(LED_RED, LOW);
    }
}


// execute factory reset
void SettingStorage::factoryReset()
{
    // Format
    InternalFS.begin();
    InternalFS.format();
}

// set A button
void SettingStorage::setA()
{
    if (m_number == -1) m_number = 0;
    
    int command = UNIT_A * GRID_MAX + m_number;
    
    if(commandTable[command][0] == 0) return; // invalid command
    
    m_unit = UNIT_A;
    m_todo = true;
}

// set B button
void SettingStorage::setB()
{
    if (m_number == -1) m_number = 0;
    
    int command = UNIT_B * GRID_MAX + m_number;
    
    if(commandTable[command][0] == 0) return; // invalid command
    
    m_unit = UNIT_B;
    m_todo = true;
}
// set Encoder
void SettingStorage::setEnc(int enc)
{
    if (m_number == -1){
        m_number = 0;
    }else{
        if(enc > 0) enc =  1;
        if(end < 0) enc = -1;
        int number = m_number + enc;
        if(number <  0)        number = GRID_MAX - 1;
        if(number >= GRID_MAX) number = 0;
    }
    
    int command = m_unit * GRID_MAX + number;
    
    if(commandTable[command][0] == 0) return; // invalid command
    
    m_number = number;
    m_todo = true;
}
// is there event to do ?
bool SettingStorage::todo()
{
    bool ret = m_todo;
    m_todo = false;
    return ret;
}
// get command length
int SettingStorage::getCommandLength()
{
    int len = 0;
    
    int command = m_unit * GRID_MAX + m_number;
    
    for(int i = 0; i < COMMAND_LEN; i++){
        if(commandTable[command][i] == 0){
            len = i;
            break;
        }
    }
    return len;
}

// get command code
uint8_t* SettingStorage::getCommandCode()
{
    int command = m_unit * GRID_MAX + m_number;
    
    for(int i = 0; i < COMMAND_LEN; i++){
        char c = commandTable[command][i];
        if(c == 0){
            m_commandCode[i] = HID_KEY_ENTER;
            break;
        }else if(c >= 'a' && c <= 'z'){
            m_commandCode[i] = HID_KEY_A + (c - 'a');
        }else if(c >= 'A' && c <= 'Z'){
            m_commandCode[i] = HID_KEY_A + (c - 'Z');
        }else if(c >= '0' && c <= '9'){
            m_commandCode[i] = HID_KEY_0 + (c - '0');
        }else if(c == ' '){
            m_commandCode[i] = HID_KEY_SPACE;
        }else if(c == ','){
            m_commandCode[i] = HID_KEY_COMMA;
        }else if(c == '.'){
            m_commandCode[i] = HID_KEY_PERIOD;
        }else{
            m_commandCode[i] = HID_KEY_ENTER;
            break;
        }
    }
    return m_commandCode;
}

// unit A or B ?
int SettingStorage::getUnit()
{
    return m_unit;
}

// get grid number (1-5)
int SettingStorage::getNumber()
{
    return m_number + 1;
}

