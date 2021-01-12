#include "Configuration.h"
#include "Device.h"

Input::Input(const uint8_t sensorPin):sensorPin(sensorPin) {
  pinMode(sensorPin, INPUT_PULLUP);
}

uint8_t Input::getState() {
  return digitalRead(sensorPin);
}


Button::Button(const uint8_t sensorPin):sensorPin(sensorPin) {
  pinMode(sensorPin, INPUT_PULLUP);
  state = State::IDLE;
  checkInput();
}

void Button::checkInput() {
 
 if (digitalRead(sensorPin) != LOW) {
    if (state == State::LONGPRESSED) state = State::IDLE;
    return;
  }
  
  if (state == State::LONGPRESSED) {
    return;
  }
  
  unsigned long StartTime = millis();
  while (digitalRead(sensorPin) == LOW) {
    if(millis() - StartTime > 1000) {
      state = State::LONGPRESSED;
      return;
    }
  }
  state = State::CLICKED;
  return;
}

uint8_t Button::getShortState() {
  if (state == State::CLICKED) {
    state = State::IDLE;
    return 1;
  }
  return 0;
}

uint8_t Button::getLongState() {
  if (state == State::LONGPRESSED) {
    return 1;
  }
  return 0;
}


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


ShutterControler::ShutterControler(const uint8_t upPin, const uint8_t downPin):upPin(upPin),downPin(downPin),OnState(RELAY_ON),OffState(RELAY_OFF) {
  pinMode(upPin, OUTPUT);  
  pinMode(downPin, OUTPUT);
  stop();
}

void ShutterControler::stop()  {
  digitalWrite(upPin, OffState);
  digitalWrite(downPin, OffState);
}

void ShutterControler::moveDown()  {
  digitalWrite(upPin, OffState);
  digitalWrite(downPin, OnState);
}

void ShutterControler::moveUp()  {
  digitalWrite(upPin, OnState);
  digitalWrite(downPin, OffState);
}
