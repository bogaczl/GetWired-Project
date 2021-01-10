#ifndef InternalTemperature_h
#define InternalTemperature_h

#include "Arduino.h"

class InternalTemperature
{
    const uint8_t sensorPin;
    const uint8_t maxTemperature;
    const float mVperC;
    const float zeroVoltage;
    bool overheat = false;
    long readVcc();
  public:
    InternalTemperature(uint8_t sensorPin, uint8_t maxTemperature=MAX_TEMPERATURE, float mVperC=MVPERC, float zeroVoltage=ZEROVOLTAGE);
    float measureT();
    bool isOverheat();
};

#endif
