/*
 * ExternalTemp.cpp
 */

#include <Wire.h>
#include <OneWire.h>               
#include "ExternalTemperature.h"


/*  *******************************************************************************************
 *                                      Constructor
 *  *******************************************************************************************/
ExternalTemperature_DHT22::ExternalTemperature_DHT22(uint8_t sensorPin)  {
  this->sensorPin=sensorPin;
  temperatureSensorCount=1;
}

int ExternalTemperature_DHT22::mapError(int errorCode) {
  switch (errorCode)  {
    case DHTLIB_OK: return 0;
    case DHTLIB_ERROR_CHECKSUM: return 1;
    case DHTLIB_ERROR_TIMEOUT: return 2;
    default: return 3;
  }
}

/*  *******************************************************************************************
 *                                      Sensor initialization
 *  *******************************************************************************************/
void ExternalTemperature_DHT22::setup() {
  pinMode(sensorPin, INPUT);
}

/*  *******************************************************************************************
 *                                      Get temperatures
 *  *******************************************************************************************/
int ExternalTemperature_DHT22::readValues() {
   return mapError(DHT.read22(sensorPin));
}

float ExternalTemperature_DHT22::getTemperature(const uint8_t i) {
    return DHT.temperature;
}






/*  *******************************************************************************************
 *                                      Constructor
 *  *******************************************************************************************/
ExternalTemperature_SHT30::ExternalTemperature_SHT30()  {
  temperatureSensorCount=1;
}

/*  *******************************************************************************************
 *                                      Sensor initialization
 *  *******************************************************************************************/
void ExternalTemperature_SHT30::setup() {
  Wire.begin();
  sht.init();
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
}

/*  *******************************************************************************************
 *                                      Get temperatures
 *  *******************************************************************************************/
int ExternalTemperature_SHT30::readValues() {
    return sht.readSample();
}

float ExternalTemperature_SHT30::getTemperature(const uint8_t i) {
    return sht.getTemperature();
}



/*  *******************************************************************************************
 *                                      Constructor
 *  *******************************************************************************************/
ExternalTemperature_DS18B20::ExternalTemperature_DS18B20(uint8_t sensorPin, byte resolution)  {
  this->sensorPin=sensorPin;
  this->resolution=resolution;
  temperatureSensorCount = 1;
  sensors = DallasTemperature(new OneWire(sensorPin));
}

/*  *******************************************************************************************
 *                                      Sensor initialization
 *  *******************************************************************************************/
void ExternalTemperature_DS18B20::setup() {
  pinMode(sensorPin, INPUT);
  sensors.setResolution(resolution);
  sensors.setWaitForConversion(true);
  sensors.begin();
}

/*  *******************************************************************************************
 *                                      Get temperatures
 *  *******************************************************************************************/
int ExternalTemperature_DS18B20::readValues() {
  temperatureSensorCount = sensors.getDS18Count();
  if (temperatureSensorCount) 
    sensors.requestTemperatures();
  return 0;
}

float ExternalTemperature_DS18B20::getTemperature(const uint8_t i) {
    if (i<temperatureSensorCount)
      return sensors.getTempCByIndex(i);
}