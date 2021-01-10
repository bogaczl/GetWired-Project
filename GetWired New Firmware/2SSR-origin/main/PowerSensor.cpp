#include "Arduino.h"
#include "PowerSensor.h"

/*  *******************************************************************************************
 *                                      Constructor
 *  *******************************************************************************************/
PowerSensor::PowerSensor(const uint8_t sensorPin, uint8_t mVperAmp, uint8_t receiverVoltage, uint8_t maxCurrent, uint8_t pMTime) : sensorPin(sensorPin),mVperAmp(mVperAmp),receiverVoltage(receiverVoltage),maxCurrent(maxCurrent),powerMeasuringTime(pMTime) {
  pinMode(sensorPin, INPUT);
  setOffset();
}

/*  *******************************************************************************************
 *                                      Set Values
 *  *******************************************************************************************/
void PowerSensor::setOffset() {
  uint16_t temp = 0;
  for(uint8_t i=0; i<10; i++)  {
    temp += analogRead(sensorPin);
  }
  zeroOffset = temp/10;
}

/*  *******************************************************************************************
 *                                   Current Measurement
 *  *******************************************************************************************/
float PowerSensor::measureAC()  {

  uint16_t readValue;
  uint16_t maxValue = 0;
  uint16_t minValue = 1024;

  uint32_t startTime = millis();
  while((millis() - startTime) < powerMeasuringTime)  {
    readValue = analogRead(sensorPin);
    if(readValue > maxValue)  {
      maxValue = readValue;
    }
    if(readValue < minValue)  {
      minValue = readValue;
    }
  }

  uint16_t maxMinValue = maxValue - minValue;
  if (maxMinValue <= 15) {
    maxMinValue = 0;
  }
  float current = (maxMinValue * readVcc() * 0.3535) / (mVperAmp * 1024.0);
  overload = (current > maxCurrent);    
  return current;
}

/*  *******************************************************************************************
 *                                   Current Measurement
 *  *******************************************************************************************/
float PowerSensor::measureDC()  {

  uint16_t n = 0;
  uint32_t averageSum = 0;
  uint32_t startTime = millis();
  while((millis() - startTime) < powerMeasuringTime)  {
    averageSum += abs(analogRead(sensorPin) - zeroOffset);
    n++;
  }

  float result = averageSum / n;
  return (result * readVcc()) / (mVperAmp * 1024.0);

}

/*  *******************************************************************************************
 *                                      Power Calculation
 *  *******************************************************************************************/
float PowerSensor::calculatePower(float current, float cosfi)  {
  return  current * receiverVoltage * cosfi;
}

/*  *******************************************************************************************
 *                                  Electrical Status Check
 *  *******************************************************************************************/
bool PowerSensor::isOverload() {
  return overload;
}

/*  *******************************************************************************************
 *                                    Read Vcc
 *  *******************************************************************************************/
long PowerSensor::readVcc() {
  
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC); 
  
  while (bit_is_set(ADCSRA,ADSC));
  
  return  1126400L / (long)(ADCL | ADCH<<8 );
}
