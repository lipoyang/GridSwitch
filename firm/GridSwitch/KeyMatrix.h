#pragma once

#include <stdbool.h>
#include <stdint.h>

// How Many rows (output pins)
#define ROW_NUM         2

// How Many cols (input pins)
#define COL_NUM         5

// How many physical keys
#define KEY_MAX         10 // 2 * 5

// Max key combination (e.g. Ctrl+Shift+A)
#define KEY_COMBI_MAX   4

// Key Matrix Class
class KeyMatrix
{
public:
    KeyMatrix();
    void begin(int outPin[], int inPin[]);
    void setKeyTable(uint8_t keyTable[][KEY_COMBI_MAX]);
    bool scan();
    void getKey(uint8_t key[]);
    bool pressAnyKey();
    bool pressFactoryReset();

private:
    int     m_outPin [ROW_NUM];
    int     m_inPin  [COL_NUM];
    uint8_t m_keyTable[KEY_MAX][KEY_COMBI_MAX];
    bool    m_pressed [KEY_MAX];
    bool    m_pressedAnyKey;
};
