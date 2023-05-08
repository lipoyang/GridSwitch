#include <Arduino.h>
#include "KeyMatrix.h"

// constructor
KeyMatrix::KeyMatrix()
{
    for(int i = 0; i < KEY_MAX; i++){
        for(int j = 0; j < KEY_COMBI_MAX; j++){
            m_keyTable[i][j] = 0; // HID_KEY_NONE;
        }
    }
}

// begin
// outPin: output pins
// inPin:  input pins
void KeyMatrix::begin(int outPin[], int inPin[])
{
    // output pins
    for(int row = 0; row < ROW_NUM; row++){
        m_outPin[row] =  outPin[row];
        pinMode         (outPin[row], OUTPUT);
        digitalWrite    (outPin[row], LOW);
    }
    
    // input pins
    for(int col = 0; col < COL_NUM; col++){
        m_inPin[col] =  inPin[col];
        pinMode        (inPin[col], INPUT_PULLDOWN);
    }
}

// set key code table
void KeyMatrix::setKeyTable(uint8_t keyTable[][KEY_COMBI_MAX])
{
    for(int i = 0; i < KEY_MAX; i++){
        for(int j = 0; j < KEY_COMBI_MAX; j++){
            m_keyTable[i][j] = keyTable[i][j];
        }
    }
}

// scan the key matrix
// return: scaned around?
bool KeyMatrix::scan()
{
    // scanning row
    static int scan_row = 0;
    
    // scaning start?
    if(scan_row == 0){
        for(int i = 0; i < KEY_MAX; i++){
            m_pressed[i] = false;
        }
        m_pressedAnyKey = false;
    }
    
    // output
    for(int row = 0; row < ROW_NUM; row++){
        if(row == scan_row){
            digitalWrite(m_outPin[row], HIGH);
        }else{
            digitalWrite(m_outPin[row], LOW);
        }
    }
    
    delay(2); // TODO ***************************
    
    // input
    for(int col = 0; col < COL_NUM; col++){
        if(digitalRead(m_inPin[col]) == HIGH){
            int key = (scan_row * COL_NUM) + col;
            m_pressed[key] = true;
            m_pressedAnyKey = true;
        }
    }
    
    // scaned around?
    scan_row++;
    if(scan_row >= ROW_NUM){
        scan_row = 0;
        return true;
    }else{
        return false;
    }
}

// get pressed key code
void KeyMatrix::getKey(uint8_t key[])
{
    for(int j = 0; j < KEY_COMBI_MAX; j++){
        key[j] = 0; // HID_KEY_NONE;
    }
    
    for(int i = 0; i < KEY_MAX; i++){
        if(m_pressed[i]){
            m_pressed[i] = false;
            
            for(int j = 0; j < KEY_COMBI_MAX; j++){
                key[j] = m_keyTable[i][j];
            }
            // !!! DON'T SUPPORT pressing of key at same time
            break;
        }
    }
}

// any key pressed?
bool KeyMatrix::pressAnyKey()
{
    return m_pressedAnyKey;
}

// factory reset keys pressed?
bool KeyMatrix::pressFactoryReset()
{
    // Key1 + Key5 pressed and other keys not pressed?
    
    // row 0
    digitalWrite(m_outPin[0], HIGH);
    digitalWrite(m_outPin[1], LOW);
    delay(2);
    
    if(digitalRead(m_inPin[0]) == LOW)  return false; // Key1
    if(digitalRead(m_inPin[1]) == HIGH) return false;
    if(digitalRead(m_inPin[2]) == HIGH) return false;
    if(digitalRead(m_inPin[3]) == HIGH) return false;
    if(digitalRead(m_inPin[4]) == LOW)  return false; // Key5
    
    // row 1
    digitalWrite(m_outPin[0], LOW);
    digitalWrite(m_outPin[1], HIGH);
    delay(2);
    
    if(digitalRead(m_inPin[0]) == HIGH) return false;
    if(digitalRead(m_inPin[1]) == HIGH) return false;
    if(digitalRead(m_inPin[2]) == HIGH) return false;
    if(digitalRead(m_inPin[3]) == HIGH) return false;
    if(digitalRead(m_inPin[4]) == HIGH) return false;
    
    return true;
}
