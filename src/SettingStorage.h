#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "SettingStorage.h"

#define GRID_MAX        5       // grid 1 - 5
#define COMMAND_MAX     10      // (grid 1 - 5) * (unit A, B) commands
#define COMMAND_LEN     32      // 31 characters max

#define UNIT_A          0       // unit A (mm)
#define UNIT_B          1       // unit B (inch)

// Setting Storage Class
class SettingStorage
{
public:
    char commandTable[COMMAND_MAX][COMMAND_LEN]; // command table
    
    void begin();           // initialize
    void load();            // load Keymap data from data Flash
    void save();            // save Keymap data to data Flash
    void factoryReset();    // execute factory reset
    
    void setA();            // set A button
    void setB();            // set B button
    void setEnc(int enc);   // set Encoder
    bool todo();            // is there event to do ?
    int      getCommandLength(); // get command length
    uint8_t* getCommandCode();   // get command code
    int getUnit();          // unit A or B ?
    int getNumber();        // get grid number (1-5)

private:
    void setDefaultValue(); // set default value
    
    uint8_t m_commandCode[COMMAND_LEN];
    bool m_todo;            // is there event to do ?
    int  m_unit;            // unit A or B ?
    int  m_number;          // get grid number (0-4) <- ZERO ORIGIN!!!
};
