#ifndef Device_h
#define Device_h

#include "Arduino.h"

class Input {
    const uint8_t sensorPin; 
  public:
    Input(const uint8_t sensorPin);
    uint8_t getState();
};

class Button {
    enum class State { IDLE, CLICKED, LONGPRESSED };
    State state = State::IDLE;
    const uint8_t sensorPin; 
  public:
    Button(const uint8_t sensorPin);
    void checkInput();
    uint8_t getShortState();
    uint8_t getLongState();
};

class Relay {
    uint8_t relayPin;
    uint8_t relayState;
    bool fuseBroken = false;
  public:
    Relay(const uint8_t relayPin);
    uint8_t getState();
    void setState(uint8_t state);
    void switchState();
    void fuseSet(bool state);
};

#endif
