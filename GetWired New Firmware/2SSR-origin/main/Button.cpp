#include "Button.h"

Button::Button(const uint8_t sensorPin):sensorPin(sensorPin) {
  pinMode(sensorPin, INPUT_PULLUP);     
}

void Button::checkInput() {
 
  if (digitalRead(sensorPin) == LOW) {
    if (state == State::LONGPRESSED) state = State::IDLE;
    return;
  }
  if (state == State::LONGPRESSED) {
    return;
  }
  
  bool longPressed = false;
  unsigned long StartTime = millis();
  
  while (digitalRead(sensorPin) != LOW) {
    if(millis() - StartTime > 1000) {
      longPressed = true;
      break; 
    }
  }
  
  if (longPressed) {
    state = State::LONGPRESSED;
    return;
  }
  if (state == State::CLICKED) {
     state = State::IDLE;
     return;
  }
  if (state == State::IDLE) {
    state = State::CLICKED;
    return;
  }
  //asert 1==0
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
