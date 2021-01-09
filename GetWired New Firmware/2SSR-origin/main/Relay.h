#ifndef Relay_h
#define Relay_h

class Relay {
    uint8_t relayPin;
    uint8_t relayState;
  public:
    Relay(const uint8_t relayPin);
    uint8_t getState();
    void setState(uint8_t state);
    void switchState();
};

#endif
