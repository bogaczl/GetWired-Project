#include "Configuration.h"
#include "Memory.h"

int Memory::initialAddress = EEPROM_CONFIG_OFFSET;

Memory::Memory(int size):size(size),begin(initialAddress) {
  initialAddress += size;
}

Memory::Memory(int size, int address):size(size),begin(address) {
  
}

void Memory::save(uint8_t value, int position) {
  if (position < size) {
    EEPROM.put(begin+position, value);
  }
}

uint8_t Memory::load(int position) {
  if (position < size) {
    uint8_t value;  
    return EEPROM.get(begin+position, value);
  }
  return 0;
}
