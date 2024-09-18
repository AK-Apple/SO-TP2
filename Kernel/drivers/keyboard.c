#include "keyboard.h"
#include "video.h"
#include "IO.h"
#include "lib.h"

#define REGS_SIZE 18

int keyFlag[4] = {0,0}; // index 0: bloq-mayus ; index 1: {1=shift ; 2=CTRL ; 3=alt}

const AsciiMap map[256] = {
        {'\x0',  0x0, 0x0, 0},  // -
        {'\x1',  0x0, 0x0, 0},
        {'\x2',  0x0, 0x0, 0},
        {'\x3',  0x2E, 0x0, 2}, // CTRL + C
        {'\x4',  0x0, 0x0, 0},
        {'\x5',  0x0, 0x0, 0},
        {'\x6',  0x0, 0x0, 0},
        {'\x7',  0x0, 0x0, 0},
        {'\x8',  0x0E, 0x0, 0}, // Backspace
        {'\x9',  0x0, 0x0, 0},
        {'\xa',  0x1C, 0x0, 0}, // Enter
        {'\xb',  0x0, 0x0, 0},
        {'\xc',  0x0, 0x0, 0},
        {'\xd',  0x0, 0x0, 0},
        {'\xe',  0x0, 0x0, 0},
        {'\xf',  0x0, 0x0, 0},
        {'\x10', 0x0, 0x0, 0},
        {'\x11', 0x0, 0x0, 0},
        {'\x12', 0x0, 0x0, 0},
        {'\x13', 0x0, 0x0, 0},
        {'\x14', 0x0, 0x0, 0},
        {'\x15', 0x0, 0x0, 0},
        {'\x16', 0x0, 0x0, 0},
        {'\x17', 0x0, 0x0, 0},
        {'\x18', 0x0, 0x0, 0},
        {'\x19', 0x0, 0x0, 0},
        {'\x1a', 0x0, 0x0, 0},
        {'\x1b', 0x0, 0x0, 0},
        {'\x1c', 0x0, 0x0, 0},
        {'\x1d', 0x0, 0x0, 0},
        {'\x1e', 0x0, 0x0, 0},
        {'\x1f', 0x0, 0x0, 0}, // -
        {'\x20', 0x39, 0xB9, 0},
        {'\x21', 0x02, 0xA8, 1},
        {'\x22', 0x03, 0x0, 1},
        {'\x23', 0x04, 0x0, 1},
        {'\x24', 0x05, 0x0, 1},
        {'\x25', 0x06, 0x0, 1},
        {'\x26', 0x07, 0x0, 1},
        {'\x27', 0x0C, 0x0, 0}, // '
        {'\x28', 0x09, 0x0, 1}, // (
        {'\x29', 0x0A, 0x0, 1}, // )
        {'\x2a', 0x1B, 0x0, 1}, // *
        {'\x2b', 0x1B, 0x0, 0}, // +
        {'\x2c', 0x33, 0xB3, 0}, // ,
        {'\x2f', 0x08, 0x8C, 1}, // - -> /
        {'\x2e', 0x34, 0xB4, 0}, // .
        {'\x2d', 0x35, 0xB5, 0}, // / -> -
        {'\x30', 0x0B, 0x8B, 0}, // 0
        {'\x31', 0x02, 0x82, 0}, // ...
        {'\x32', 0x03, 0x83, 0},
        {'\x33', 0x04, 0x84, 0},
        {'\x34', 0x05, 0x85, 0},
        {'\x35', 0x06, 0x86, 0},
        {'\x36', 0x07, 0x87, 0},
        {'\x37', 0x08, 0x88, 0},
        {'\x38', 0x09, 0x89, 0}, // ...
        {'\x39', 0x0A, 0x8A, 0}, // 9
        {'\x3a', 0x34, 0x0, 1}, // :
        {'\x3b', 0x33, 0xA7, 1}, // ;
        {'\x3c', 0x56, 0x0, 0}, // REV <
        {'\x3d', 0x0B, 0x8D, 1}, // =
        {'\x3e', 0x56, 0x0, 1}, // REV >
        {'\x3f', 0x0C, 0x0, 1}, // ?
        {'\x40', 0x10, 0x0, 3},
        {'\x41', 0x1E, 0x9E, 0},
        {'\x42', 0x30, 0xB0, 0},
        {'\x43', 0x2E, 0xAE, 0},
        {'\x44', 0x20, 0xA0, 0},
        {'\x45', 0x12, 0x92, 0},
        {'\x46', 0x21, 0xA1, 0},
        {'\x47', 0x22, 0xA2, 0},
        {'\x48', 0x23, 0xA3, 0},
        {'\x49', 0x17, 0x97, 0},
        {'\x4a', 0x24, 0xA4, 0},
        {'\x4b', 0x25, 0xA5, 0},
        {'\x4c', 0x26, 0xA6, 0},
        {'\x4d', 0x32, 0xB2, 0},
        {'\x4e', 0x31, 0xB1, 0},
        {'\x4f', 0x18, 0x98, 0},
        {'\x50', 0x19, 0x99, 0},
        {'\x51', 0x10, 0x90, 0},
        {'\x52', 0x13, 0x93, 0},
        {'\x53', 0x1F, 0x9F, 0},
        {'\x54', 0x14, 0x94, 0},
        {'\x55', 0x16, 0x96, 0},
        {'\x56', 0x2F, 0xAF, 0},
        {'\x57', 0x11, 0x91, 0},
        {'\x58', 0x2D, 0xAD, 0},
        {'\x59', 0x15, 0x95, 0},
        {'\x5a', 0x2C, 0xAC, 0},
        {'\x5b', 0x28, 0x9A, 1},
        {'\x5c', 0x0C, 0xAB, 3},
        {'\x5d', 0x2B, 0x9B, 1},
        {'\x5e', 0x28, 0x0, 3},
        {'\x5f', 0x35, 0x0, 1}, // _
        {'\x60', 0x2B, 0xA9, 3},
        {'\x61', 0x1E, 0x9E, 0},  // 'a'
        {'\x62', 0x30, 0xB0, 0},  // 'b'
        {'\x63', 0x2E, 0xAE, 0},  // 'c'
        {'\x64', 0x20, 0xA0, 0},  // 'd'
        {'\x65', 0x12, 0x92, 0},  // 'e'
        {'\x66', 0x21, 0xA1, 0},  // 'f'
        {'\x67', 0x22, 0xA2, 0},  // 'g'
        {'\x68', 0x23, 0xA3, 0},  // 'h'
        {'\x69', 0x17, 0x97, 0},  // 'i'
        {'\x6A', 0x24, 0xA4, 0},  // 'j'
        {'\x6B', 0x25, 0xA5, 0},  // 'k'
        {'\x6C', 0x26, 0xA6, 0},  // 'l'
        {'\x6D', 0x32, 0xB2, 0},  // 'm'
        {'\x6E', 0x31, 0xB1, 0},  // 'n'
        {'\x6F', 0x18, 0x98, 0},  // 'o'
        {'\x70', 0x19, 0x99, 0},  // 'p'
        {'\x71', 0x10, 0x90, 0},  // 'q'
        {'\x72', 0x13, 0x93, 0},  // 'r'
        {'\x73', 0x1F, 0x9F, 0},  // 's'
        {'\x74', 0x14, 0x94, 0},  // 't'
        {'\x75', 0x16, 0x96, 0},  // 'u'
        {'\x76', 0x2F, 0xAF, 0},  // 'v'
        {'\x77', 0x11, 0x91, 0},  // 'w'
        {'\x78', 0x2D, 0xAD, 0},  // 'x'
        {'\x79', 0x15, 0x95, 0},  // 'y'
        {'\x7A', 0x2C, 0xAC, 0},  // 'z'
        {'\x7b', 0x28, 0x0, 0},
        {'\x7c', 0x29, 0x0, 0},
        {'\x7d', 0x2B, 0x0, 0},
        {'\x7e', 0x1B, 0x0, 3},
        {'\x7f', 0x29, 0x0, 1},
        {'\xa4', 0xA7, 0x0, 0}, // ñ
        {'\xac', 0x29, 0x0, 3},
        {'\xd1', 0x27, 0x0, 1},
        {'\xf1', 0x27, 0x0, 0},
        {'\xFF', 0x54, 0x0, 0}  // guarda los registros
};

char mapKey(char character, int flags[2]) {
    for (int i = 0 ; i < 131 ; i++) {
        if (map[i].make_code == character) {
            if (map[i].character >= 'A' && map[i].character <= 'Z') {  // es una letra
                if (!flags[0]) {    // se fija si mayus esta desactivado (si es 0)
                    return map[i].character + 'a' - 'A';
                }
                return map[i].character;
            }
            if (map[i].specialFlag == flags[1]) {
                return map[i].character;
            }
        }
    }
    return '\0';
}

void keyboard_handler() {
    char i = getKey();      // llamada a Assembler
    char key = mapKey(i, keyFlag);
    switch (i) {
        case '\x3A':    // bloq-mayus
            keyFlag[0] = !keyFlag[0];
            break;
        case '\x36':    // right-shift pressed
        case '\x2A':    // left-shift pressed
            keyFlag[0] = !keyFlag[0];
            keyFlag[1] = 1;
            break;
        case '\xAA':    // left-shift released
        case '\xB6':    // right-shift released
            keyFlag[0] = !keyFlag[0];
            keyFlag[1] = 0;
            break;
        case '\x9D':    // left-ctrl released
        case '\xB8':    // left-alt released
            keyFlag[1] = 0;
            break;
        case '\x1D':    // left-ctrl pressed
            keyFlag[1] = 2;
            break;
        case '\x38':    // left-alt pressed (guarda registros)
            keyFlag[1] = 3;
            getRegs();
            break;
        default:
            if (key != '\0') {
                putIn(key);
            }
            break;
    }
}