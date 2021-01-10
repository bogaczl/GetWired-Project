#ifndef PowerSensor_h
#define PowerSensor_h

#include "Configuration.h"

class PowerSensor {
    const uint8_t sensorPin;
    uint8_t receiverVoltage;
    uint8_t maxCurrent;
    uint8_t powerMeasuringTime;
    uint16_t zeroOffset;
    uint8_t mVperAmp;
    bool overload = false;
    void setOffset();
  public:
    PowerSensor(const uint8_t sensorPin, uint8_t mVperAmp=MVPERAMP, uint8_t receiverVoltage=RECEIVER_VOLTAGE, uint8_t maxCurrent=MAX_CURRENT, uint8_t pMTime=POWER_MEASURING_TIME);
    float measureAC();
    float measureDC();
    float calculatePower(float current, float cosfi);
    bool isOverload();
    long readVcc();
};

#endif
