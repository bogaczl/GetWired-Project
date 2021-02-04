#include "Configuration.h"
#include "Memory.h"

Memory::Memory(int size, int address):size(size),begin(EEPROM_OFFSET + address) {

}

Memory& Memory::save(uint8_t value, int position) {
  if (position < size) {
    EEPROM.put(begin+position, value);
  }
  return (*this);
}

void Memory::saveString(const char * buf) {
  int i = 0;
  do {
    save(buf[i],i);
  }
  while ((buf[i] != '\0') && (buf[i] != ' ') && (++i < size));
}

uint8_t Memory::load(int position) {
  if (position < size) {
    uint8_t value;  
    return EEPROM.get(begin+position, value);
  }
  return 0;
}

char * Memory::loadString(char * buf) {
  int i = 0;
  do {
    buf[i] = load(i);
  }
  while ((buf[i] != '\0') && (buf[i] != ' ') && (i++ < size));
  buf[i]='\0';
  return buf;
}

char * Memory::loadString(char * buf, char * defaultValue) {
  if (load()==255) {
    sprintf(buf,"%s",defaultValue);
  }
  else {
    loadString(buf);
  }
  return buf;
}
