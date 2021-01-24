#include "Configuration.h"
#include "Device.h"

Input::Input(const uint8_t sensorPin):sensorPin(sensorPin) {
  pinMode(sensorPin, INPUT_PULLUP);
}

uint8_t Input::getState() {
  return digitalRead(sensorPin);
}


Button::Button(const uint8_t sensorPin):sensorPin(sensorPin),previousClickTime(0) {
  pinMode(sensorPin, INPUT_PULLUP);
  state = State::IDLE;
  checkInput();
}

void Button::checkInput() {
 if (digitalRead(sensorPin) != LOW) {
    if (state == State::LONG_PRESSED) state = State::IDLE;
    return;
  }
  
  if (state == State::LONG_PRESSED) {
    return;
  }
  
  unsigned long startTime = millis();
  while (digitalRead(sensorPin) == LOW) {
    if(millis() - startTime > 1000) {
      longPressTime = startTime;
      state = State::LONG_PRESSED;
      return;
    }
  }
  if (millis() - previousClickTime < 300) {
    state = State::DOUBLE_CLICKED;
    previousClickTime = 0;
    return;
  }
  previousClickTime = millis();
  state = State::CLICKED;
  return;
}

bool Button::getShortState() {
  if (state == State::CLICKED) {
    state = State::IDLE;
    return true;
  }
  return false;
}

bool Button::getDoubleState() {
  if (state == State::DOUBLE_CLICKED) {
    state = State::IDLE;
    return true;
  }
  return false;
}

bool Button::getLongState() {
  if (state == State::LONG_PRESSED) {
    return true;
  }
  return false;
}

unsigned long Button::getLongPressTime() {
   return millis() - longPressTime;
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
  digitalWrite(upPin, OffState);  
  pinMode(downPin, OUTPUT);
  digitalWrite(downPin, OffState);
}

void ShutterControler::stop()  {
  digitalWrite(upPin, OffState);
  digitalWrite(downPin, OffState);
}

void ShutterControler::goDown()  {
  digitalWrite(upPin, OffState);
  delay(10);
  digitalWrite(downPin, OnState);
}

void ShutterControler::goUp()  {
  digitalWrite(downPin, OffState);
  delay(10);
  digitalWrite(upPin, OnState);
}


SmartShutterControler::SmartShutterControler(ShutterControler * shutterControler, int position=0, int upTime=0, int downTime=0):shutterControler(shutterControler),position(position),upTime(upTime),downTime(downTime) {
    
}

void SmartShutterControler::updatePosition() {
  unsigned long trawelTime = millis() - startTime;
  if ((state == State::GOING_UP) && (upTime)) {
    position -= (100.0*trawelTime)/upTime;
    if (position < 0) 
      position = 0;
  }
  if ((state == State::GOING_DOWN) && (downTime)) {
    position += (100.0*trawelTime)/downTime;
    if (position > 100) 
      position = 100;
  }
}
   
void SmartShutterControler::goUp() {
  if (state != State::GOING_UP) {
    shutterControler->stop();
  }
  startTime = millis();
  state = State::GOING_UP;
}

void SmartShutterControler::goDown() {
  if (state != State::GOING_DOWN) {
    shutterControler->stop();
  }
  shutterControler->goDown();
  startTime = millis();
  state = State::GOING_DOWN;  
}

void SmartShutterControler::stop() {
  shutterControler->stop();
  updatePosition();
  state = State::IDLE;
}

int SmartShutterControler::getPosition() {
  if (!isCalibrated()) {
    if (state == State::GOING_UP) 
    {
      position = 0;
    }
    if (state == State::GOING_DOWN) { 
      position = 100; 
    }
  }
  else
    updatePosition();
  return position;
}

void SmartShutterControler::goPosition(int position) {
  if (!isCalibrated()) return;
}

void SmartShutterControler::calibrate(int upTime, int downTime, int position) {
  this->upTime = upTime + 1000;
  this->downTime = downTime + 1000;
  this->position = position;
}

bool SmartShutterControler::isCalibrated() {
  return (upTime && downTime);
}

SmartShutterControler::State SmartShutterControler::getState() {
  return state;
}
