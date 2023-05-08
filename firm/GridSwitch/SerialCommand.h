#pragma once

#include <stdio.h>
#include <string.h>
#include "KeyMap.h"

// return value of Serial Command task
#define RET_OK      0   // OK, nothing to do
#define RET_WRITE   1   // OK, renew key table and save keymap data.
#define RET_ERROR   -1  // Error!

// Serial Command Class
class SerialCommand
{
public:
    KeyMap *keyMaps;        // Keymap data
    
    void begin();           // begin Serial Command
    int  task();            // Serial Command task

private:
    int      recvCnt;       // Received byte counter
    uint8_t  telegram[256]; // Buffer for receiving telegram
    uint8_t  len;           // Expected length of receiving telegram
    uint32_t t0;            // Start time of receiveing (for Timeout)
    
    // Keymap object <-> Byte array
    bool BytesToKeyMap(KeyMap *keyMap, uint8_t *bData);
    void KeyMapToBytes(KeyMap *keyMap, uint8_t *bData);
    
    // send response
    void sendResponse(uint8_t command, uint8_t* data, int dataLen);
    void sendResponse(uint8_t command);
};
