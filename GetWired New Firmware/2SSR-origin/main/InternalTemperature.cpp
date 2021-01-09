#include "Configuration.h"
#include "InternalTemperature.h"

/*  *******************************************************************************************
 *                                      Constructor
 *  *******************************************************************************************/
InternalTemperature::InternalTemperature(uint8_t sensorPin, uint8_t maxTemperature, float mVperC, float zeroVoltage):sensorPin(sensorPin),maxTemperature(maxTemperature),mVperC(mVperC),zeroVoltage(zeroVoltage) {
	pinMode(sensorPin, INPUT);  
}

/*  *******************************************************************************************
 *                                 Temperature measurement
 *  *******************************************************************************************/
float InternalTemperature::measureT()  {
  int readValue = analogRead(sensorPin);
  float vcc = readVcc();
  return ((readValue * vcc) / 1024.0 - zeroVoltage) / mVperC;
}

/*  *******************************************************************************************
 *                                  Thermal Status Check
 *  *******************************************************************************************/
bool InternalTemperature::thermalStatus(float temperature)  {
  return (temperature > maxTemperature);
}

/*  *******************************************************************************************
 *                                    Read Vcc
 *  *******************************************************************************************/
long InternalTemperature::readVcc() {
  
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC); 
  
  while (bit_is_set(ADCSRA,ADSC));
  
  return  1126400L / (long)(ADCL | ADCH<<8 );
}
