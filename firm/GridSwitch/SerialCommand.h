#pragma once

#include <stdio.h>
#include <string.h>
#include "SettingStorage.h"

// return value of Serial Command task
#define RET_OK      0   // OK, nothing to do
#define RET_WRITE   1   // OK, please save data
#define RET_ERROR   -1  // Error!

// Serial Command Class
class SerialCommand
{
public:
    char (*commandTable)[COMMAND_LEN]; // command table
    
    void begin();           // begin Serial Command
    int  task();            // Serial Command task

private:
    int      recvCnt;       // Received byte counter
    uint8_t  telegram[64];  // Buffer for receiving telegram
    uint8_t  len;           // Expected length of receiving telegram
    uint32_t t0;            // Start time of receiveing (for Timeout)
    
    // send response
    void sendResponse(uint8_t command, uint8_t* data, int dataLen);
    void sendResponse(uint8_t command);
};
