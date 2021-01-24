#ifndef Memory_h
#define Memory_h

#include "Arduino.h"
#include <EEPROM.h>

class Memory {
    static int initialAddress;
    const int begin;
    const int size;
public:
    Memory(int size);
    Memory(int size, int address);
    void save(uint8_t value, int position = 0);
    uint8_t load(int position = 0);
};

#endif
