#include "Configuration.h"
#include "Memory.h"

int Memory::initialAddress = EEPROM_OFFSET;

Memory::Memory(int size):size(size),begin(initialAddress) {
  initialAddress += size;
}

Memory::Memory(int size, int address):size(size),begin(EEPROM_OFFSET + address) {

}

void Memory::save(uint8_t value, int position) {
  if (position < size) {
    EEPROM.put(begin+position, value);
  }
}

void Memory::saveString(char * name) {
  int i = 0;
  while ((name[i] != '\0') && (i < 20)) {
     save(name[i],i);
  }
}

uint8_t Memory::load(int position) {
  if (position < size) {
    uint8_t value;  
    return EEPROM.get(begin+position, value);
  }
  return 0;
}

char * Memory::loadString() {
  static char name[20];
  int i = 0;
  do {
    name[i] = load(i);
  }
  while ((name[i] != '\0') && (i < size));
  return name;
}

char * Memory::loadString(char * defaultValue) {
  return (load()==255) ? defaultValue : loadString();
}
