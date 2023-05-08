#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "KeyMatrix.h"

// How many keymap layers
#define PAGE_MAX      8

// Keymap data
struct KeyMap
{
    bool    Enabled;            // use this layer?
    uint8_t Name[20];           // Name (not used in this device side)
    uint8_t Led[3];             // LED color {R,G,B}
    uint8_t Modifiers[KEY_MAX]; // key modifiers [physical key number]
    uint8_t KeyCodes [KEY_MAX]; // key code      [physical key number]
};

// Keymap Storage Class
class KeyMapStorage
{
public:
    KeyMap keyMaps[PAGE_MAX]; // Keymap data
    
    void begin();           // initialize
    void changeLayer(bool); // change Keymap Layer
    void load();            // load Keymap data from data Flash
    void save();            // save Keymap data to data Flash

    uint32_t getLedColor(); // get LED color
    void getKeyTable(uint8_t keyTable[][KEY_COMBI_MAX] ); // get Key matrix table
    
    void factoryReset();    // execute factory reset

private:
    int               m_layer;   // Keymap Layer
    
    void setDefaultValue(); // set default value
};
