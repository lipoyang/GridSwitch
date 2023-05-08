/********** キーコードの定数 ***********/

const KeyCodeTable = {
    // A-Z
    "A":          0x04,
    "B":          0x05,
    "C":          0x06,
    "D":          0x07,
    "E":          0x08,
    "F":          0x09,
    "G":          0x0A,
    "H":          0x0B,
    "I":          0x0C,
    "J":          0x0D,
    "K":          0x0E,
    "L":          0x0F,
    "M":          0x10,
    "N":          0x11,
    "O":          0x12,
    "P":          0x13,
    "Q":          0x14,
    "R":          0x15,
    "S":          0x16,
    "T":          0x17,
    "U":          0x18,
    "V":          0x19,
    "W":          0x1A,
    "X":          0x1B,
    "Y":          0x1C,
    "Z":          0x1D,
    // 1-9,0
    "1":          0x1E,
    "2":          0x1F,
    "3":          0x20,
    "4":          0x21,
    "5":          0x22,
    "6":          0x23,
    "7":          0x24,
    "8":          0x25,
    "9":          0x26,
    "0":          0x27,
    // 制御キー
    "Enter":      0x28,
    "Esc":        0x29,
    "Backspace":  0x2A,
    "Tab":        0x2B,
    "Space":      0x2C,
    // 記号
    "-":          0x2D,
    "=":          0x2E,
    "[":          0x2F,
    "]":          0x30,
    "\\":         0x31, // バックスラッシュ
    ";":          0x33,
    "'":          0x34,
    "`":          0x35,
    ":":          0x36,
    ".":          0x37,
    "/":          0x38,
    // F1-F12
    "F1":         0x3A,
    "F2":         0x3B,
    "F3":         0x3C,
    "F4":         0x3D,
    "F5":         0x3E,
    "F6":         0x3F,
    "F7":         0x40,
    "F8":         0x41,
    "F9":         0x42,
    "F10":        0x43,
    "F11":        0x44,
    "F12":        0x45,
    // 制御キー
    "Pause":      0x48,
    "Insert":     0x49,
    "Home":       0x4A,
    "PageUp":     0x4B,
    "Delete":     0x4C,
    "End":        0x4D,
    "PageDown":   0x4E,
    "Right":      0x4F,
    "Left":       0x50,
    "Down":       0x51,
    "Up":         0x52,
};

/********** キーの名前 ⇔ キーコード の変換 ***********/

// キーの名前 → キーコード
function getKeyCode(name){
    if (name in KeyCodeTable) {
        return KeyCodeTable[name];
    }else{
        console.log("ERROR 不正なキー名:" + name);
        return 0x00;
    }
}

// キーコード → キーの名前
function getKeyName(code){
    for (let name in KeyCodeTable) {
        if(KeyCodeTable[name] == code) return name;
    }
    console.log("ERROR 不正なキーコード:" + code.toString(16));
    return "";
}
