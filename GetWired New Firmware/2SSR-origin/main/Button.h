#ifndef Button_h
#define Button_h

#include "Arduino.h"

class Button {
    enum class State { IDLE, CLICKED, LONGPRESSED };
    State state = State::IDLE;
    uint8_t sensorPin; 
  public:
    Button(const uint8_t sensorPin);
    void checkInput();
    uint8_t getShortState();
    uint8_t getLongState();
};

#endif
