#include "Arduino.h"
#include "Configuration.h"
#include "Relay.h"

Relay::Relay(const uint8_t relayPin):relayPin(relayPin) {
  relayState = RELAY_OFF;
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);  
}

uint8_t Relay::getState() {
  return relayState;
}

void Relay::setState(uint8_t state) {
  if (relayState == state) {
    return;  
  }
  digitalWrite(relayPin, state);
  relayState = state;
}

void Relay::switchState() {
  (relayState == RELAY_OFF) ? setState(RELAY_ON) : setState(RELAY_OFF);
}
