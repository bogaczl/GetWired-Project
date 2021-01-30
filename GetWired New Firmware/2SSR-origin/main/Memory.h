#ifndef Memory_h
#define Memory_h

#include "Arduino.h"
#include <EEPROM.h>

#define EEPROM_OFFSET 512

#define SHIFT_NODENAME 0
#define SHIFT_NODECONFIG 20
#define SHIFT_RS 100

class Memory {
  static int initialAddress;
  const int begin;
  const int size;
public:
  Memory(int size);
  Memory(int size, int address);
  void save(uint8_t value, int position = 0);
  uint8_t load(int position = 0);
  void saveString(char * name);
  char * loadString();
  char * loadString(char * defaultValue);
};

#endif
