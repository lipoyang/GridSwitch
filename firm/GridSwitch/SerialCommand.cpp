#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "SerialCommand.h"

// Command constatns
#define COM_READ      0x10 // READ command
#define COM_WRITE     0x20 // WRITE command
#define ERR_CHECKSUM  0x80 // ERROR: Checksum missmatch
#define ERR_COMMAND   0x81 // ERROR: Invalid Command
#define ERR_PARAM     0x82 // ERROR: Invalid Parameter

// Response size of READ command
#define READ_DATA_SIZE  45

// Byte array -> Keymap object
bool SerialCommand::BytesToKeyMap(KeyMap *keyMap, uint8_t *bData)
{
    // Enabled?
    keyMap->Enabled = (bData[0] == 0) ? false : true;
    // Name
    for(int i = 0; i < 20; i++){
        keyMap->Name[i] = bData[1 + i];
    }
    // LED color
    keyMap->Led[0] = bData[21];
    keyMap->Led[1] = bData[22];
    keyMap->Led[2] = bData[23];
    // Each Key
    for(int key = 0; key < KEY_MAX; key++){
        keyMap->Modifiers[key] = bData[24 + 2 * key];
        keyMap->KeyCodes [key] = bData[25 + 2 * key];
    }
    return true;
}

// Keymap object -> Byte array
void SerialCommand::KeyMapToBytes(KeyMap *keyMap, uint8_t *bData)
{
    // Enabled?
    bData[0] = keyMap->Enabled ? 0x01 : 0x00;
    // Name
    for(int i = 0; i < 20; i++){
        bData[1 + i] = keyMap->Name[i];
    }
    // LED color
    bData[21] = keyMap->Led[0];
    bData[22] = keyMap->Led[1];
    bData[23] = keyMap->Led[2];
    // Each Key
    for(int key = 0; key < KEY_MAX; key++){
        bData[24 + 2 * key] = keyMap->Modifiers[key];
        bData[25 + 2 * key] = keyMap->KeyCodes [key];
    }
}

// send response
// command: command code
// data: data buffer
// dataLen: data length
void SerialCommand::sendResponse(uint8_t command, uint8_t* data, int dataLen)
{
    static uint8_t telegram[256];
    
    // generate telegram
    telegram[0] = 0xAA;
    telegram[1] = 0x55;
    telegram[2] = dataLen + 2;
    telegram[3] = command;
    //   data
    for (int i = 0; i < dataLen; i++) {
        telegram[4 + i] = data[i];
    }
    //   checksum
    uint8_t sum = 0;
    for (int i = 0; i < 4 + dataLen; i++) {
    sum += telegram[i];
    }
    telegram[4 + dataLen] = sum & 0xFF;

    // send telegram
    Serial.write(telegram, dataLen + 5);
}

// send response
// command: command code
void SerialCommand::sendResponse(uint8_t command)
{
    sendResponse(command, NULL, 0);
}

// begin
void SerialCommand::begin()
{
    // begin Serial port
    Serial.begin(115200);
    delay(100);
    
    // initialize variables
    recvCnt = 0;
}

// Serial Command task
int SerialCommand::task()
{
    bool isError  = false;  // is error？
    bool received = false;  // has a telegram received？

    // timeout?
    if(recvCnt > 0){
      uint32_t now = millis();
      uint32_t elapsed = now - t0;
      if(elapsed > 1000){
        recvCnt = 0;
      }
    }
    
    // received data?
    while(Serial.available() > 0)
    {
        // get received byte
        uint8_t bData = Serial.read();
        telegram[recvCnt] = bData;
        
        // 1st byte: AA
        if(recvCnt == 0){
            if(bData != 0xAA){
                isError = true;
                break;
            }
            t0 = millis(); // start time (for timeout)
        }
        // 2nd byte: 55
        else if(recvCnt == 1){
            if(bData != 0x55){
                isError = true;
                break;
            }
        }
        // 3rd byte: Expected Length
        else if(recvCnt == 2){
            len = bData;
        }
        // Last byte
        else if(recvCnt == len + 2){
            telegram[recvCnt] = bData;
            // verify checksum
            uint8_t sum = 0;
            for(int j = 0; j < len + 2; j++){ sum += telegram[j]; }
            if(sum != bData){
                isError = true;
                sendResponse(ERR_CHECKSUM); // error response
                break;
            }else{
                received = true;
            }
        }
        recvCnt++;
    }
    // if error, reset receiving counter
    if(isError) goto _ERROR;
    
    // if received a telegram
    if(received){
        recvCnt = 0; // reset receiving counter
        
        uint8_t  command =  telegram[3]; // Command code
        uint8_t  layer   =  telegram[4]; // Layer number
        uint8_t *bData; // bynary array of Keymap data
        // check layer number
        if(layer >= PAGE_MAX){
            sendResponse(ERR_PARAM); // error response
            goto _ERROR;
        }
        switch(command){
            case COM_READ: // Device -> PC
            {
                // Byte array -> Keymap object
                static uint8_t data[64];
                data[0] = layer;
                bData = &data[1];
                KeyMapToBytes(&keyMaps[layer], bData);
                // send response
                sendResponse(COM_READ, data, READ_DATA_SIZE);
                break;
            }
            case COM_WRITE: // Device <- PC
            {
                // Keymap object -> Byte array
                bData = &telegram[5];
                BytesToKeyMap(&keyMaps[layer], bData);
                // send response
                sendResponse(COM_WRITE, &layer, 1);
                
                // last layer?
                if(layer == PAGE_MAX - 1)
                {
                    return RET_WRITE;
                }
                break;
            }
            default:
            {
                sendResponse(ERR_COMMAND); // error response
                goto _ERROR;
            }
        }
    }
    return RET_OK;
    
_ERROR:
    recvCnt = 0;
    return RET_ERROR;
}
