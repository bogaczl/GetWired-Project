#ifndef ExternalTemperature_h
#define ExternalTemperature_h

#include "Arduino.h"
#include <dht.h>
#include "SHTSensor.h"
#include <DallasTemperature.h>

/*  *******************************************************************************************
 *    ExternalTemp - base class for tempereture sensors.  
 *      
 *  *******************************************************************************************/
class ExternalTemperature {
  protected:
      int temperatureSensorCount = 0;
  public:
    ExternalTemperature() { };
    // setup() - should be called before first read
    virtual void setup() = 0;
    // readValues() - update values from sensors. Returns non-zero error code when read is not possible.
    virtual int readValues() = 0;
    // getLastSensorCount() - returns number of sensors detected on a wire. Supported e.g. by DS18B20
    virtual int getLastSensorCount() { return temperatureSensorCount; }
    // getLastSensorCount() - get temperature from last readout
    virtual float getTemperature(const uint8_t i=0) = 0;
    // check if sensors supports addidtional humidity measurements
    virtual bool isHumiditySupported() { return false; }
    // getLastSensorCount() - get humidity from last readout
    virtual float getHumidity() { return 0.0; }
};

/*  *******************************************************************************************
 *    ExternalTemp_DHT22 - implementation for DHT22 sensor
 *      
 *  *******************************************************************************************/
class ExternalTemperature_DHT22 : public ExternalTemperature {
      dht DHT;
      uint8_t sensorPin;
      int mapError(int errorCode);
  public:
      ExternalTemperature_DHT22(uint8_t sensorPin);
      void setup();
      int readValues();
      float getTemperature(const uint8_t i=0);
      bool isHumiditySupported() { return true; }
      virtual float getHumidity() { return DHT.humidity; }
};


/*  *******************************************************************************************
 *    ExternalTemp_SHT30 - implementation for SHT30 sensor
 *      
 *  *******************************************************************************************/
class ExternalTemperature_SHT30 : public ExternalTemperature {
      SHTSensor sht;
  public:
      ExternalTemperature_SHT30 ();
      void setup();
      int readValues();
      float getTemperature(const uint8_t i=0);
      bool isHumiditySupported() { return true; }
      virtual float getHumidity() { return sht.getHumidity(); }      
};


/*  *******************************************************************************************
 *    ExternalTemp_DS18B20 - implementation for DS18B20 sensors on 1-wire
 *      
 *  *******************************************************************************************/
class ExternalTemperature_DS18B20 : public ExternalTemperature {
      DallasTemperature sensors;
      uint8_t sensorPin;
      byte resolution; // global resolution to 9, 10, 11, or 12 bits
  public:
      ExternalTemperature_DS18B20(uint8_t sensorPin, byte resolution = 12);
      void setup();
      int readValues();
      float getTemperature(const uint8_t i=0);
};

#endif
