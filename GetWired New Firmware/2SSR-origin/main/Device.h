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
    unsigned long previousClickTime;
    unsigned long longPressTime;
    enum class State { IDLE, CLICKED, DOUBLE_CLICKED, LONG_PRESSED };
    State state = State::IDLE;
    const uint8_t sensorPin; 
  public:
    Button(const uint8_t sensorPin);
    void checkInput();
    bool getShortState();
    bool getDoubleState();
    bool getLongState();
    unsigned long getLongPressTime();
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

class ShutterControler {
    uint8_t upPin;
    uint8_t downPin;
    bool OnState;
    bool OffState;
  public:
    ShutterControler(const uint8_t upPin, const uint8_t downPin);
    void stop();
    void goUp();
    void goDown();
};

class SmartShutterControler {
  public:
    enum class State { IDLE, GOING_UP, GOING_DOWN };
  private:    
    int upTime;
    int downTime;
    int position;
    unsigned long startTime;
    State state = State::IDLE;
    const ShutterControler * shutterControler;
    void updatePosition();
  public:
    SmartShutterControler(ShutterControler * shutterControler, int position=0, int upTime=0, int downTime=0);
    void goUp();
    void goDown();
    void stop();
    int getPosition();
    void goPosition(int position);
    void calibrate(int upTime, int downTime, int position);
    bool isCalibrated();
    State getState();
};

#endif
