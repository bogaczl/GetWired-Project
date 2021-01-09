#include "Arduino.h"
#include <core/MySensorsCore.h>
#include "Relay.h"
#include "Button.h"
#include "PowerSensor.h"
#include "InternalTemperature.h"
#include "ExternalTemperature.h"
#include "MySensorDevice.h"

/*  *******************************************************************************************
 *                                    MySensor device decorator
 *  *******************************************************************************************/
int MySensorDevice::usedIDs = 0;
MySensorDevice::MySensorDevice(const mysensors_sensor_t sensorType, const char *description,int size):sensorId(usedIDs),sensorType(sensorType),description(description) {
  usedIDs += size;
}

void MySensorDevice::presentDevice() {  
  present(sensorId, sensorType, description);   
}

void MySensorDevice::processMessage(const MyMessage &) { }

/*  *******************************************************************************************
 *                                    MySensor button
 *  *******************************************************************************************/
MySensorButton::MySensorButton(const char *description, Button * button):MySensorDevice(S_BINARY, description, 2),button(button) { 
    previousLongState = 0;
    msgShort = new MyMessage(sensorId, V_LIGHT);
    msgLong = new MyMessage(sensorId+1,V_LIGHT);
}

void MySensorButton::presentDevice() {
  char con[40];
  sprintf(con,"%s %s",description, "short");  
  present(sensorId, S_BINARY, con);
  sprintf(con,"%s %s",description, "long");  
  present(sensorId+1, S_BINARY, con);   
}

void MySensorButton::initDevice() {
  send(msgShort->set(button->getShortState()));
  send(msgLong->set(button->getLongState()));
}

void MySensorButton::updateDevice() {
  uint8_t state = button->getShortState();
  if (state) {
    send(msgShort->set("1"));
    delay(100);
    send(msgShort->set("0"));
  }
  state = button->getLongState();
  if (previousLongState != state) {
    send(msgLong->set(state));
    previousLongState = state;
  }
}

/*  *******************************************************************************************
 *                                    MySensor relay
 *  *******************************************************************************************/
MySensorRelay::MySensorRelay(const char *description, Relay * relay):MySensorDevice(S_BINARY, description),relay(relay) {
  msg = new MyMessage(sensorId, V_LIGHT);
}

void MySensorRelay::presentDevice() {
  present(sensorId, S_BINARY, description);
}

void MySensorRelay::initDevice() {
  previousState = relay->getState();
  send(msg->set(previousState));
  request(sensorId, V_STATUS);
  wait(2000, C_SET, V_STATUS);
}

void MySensorRelay::updateDevice() {
  if (previousState != relay->getState()) {
    send(msg->set(relay->getState()));
    previousState = relay->getState();
  }
}

void MySensorRelay::processMessage(const MyMessage &message) {
  if (message.sensor != sensorId) return;
  if (message.type == V_STATUS)
    relay->setState(message.getBool());
}

/*  *******************************************************************************************
 *                                    MySensor relay paired with button
 *  *******************************************************************************************/
MySensorRelayButton::MySensorRelayButton(const char *description, Button * button, Relay * relay):MySensorButton(description, button),relay(relay) {
  
}

void MySensorRelayButton::presentDevice() {
  MySensorButton::presentDevice();
}

void MySensorRelayButton::initDevice() {
  MySensorButton::initDevice();
}

void MySensorRelayButton::updateDevice() {
  uint8_t state = button->getShortState();
  if (state) {
    relay->switchState();
    send(msgShort->set("1"));
    delay(100);
    send(msgShort->set("0"));
  }
  state = button->getLongState();
  if (previousLongState != state) {
    send(msgLong->set(state));
    previousLongState = state;
  }
}


/*  *******************************************************************************************
 *                                    MySensor power sensor
 *  *******************************************************************************************/
MySensorPowerSensor::MySensorPowerSensor(const char *description, PowerSensor * sensor, Relay * relay1, Relay * relay2): MySensorDevice(S_POWER, description),powerSensor(sensor),relay1(relay1),relay2(relay2) {
  msg = new MyMessage(sensorId, V_WATT);
}

void MySensorPowerSensor::initDevice() {
  send(msg->set("0"));
}

void MySensorPowerSensor::updateDevice() {
  
}


/*  *******************************************************************************************
 *                                    MySensor internal temperature
 *  *******************************************************************************************/
MySensorInternalTemperature::MySensorInternalTemperature(const char *description, InternalTemperature * sensor, Relay * relay1, Relay * relay2): MySensorDevice(S_TEMP, description),sensor(sensor),relay1(relay1),relay2(relay2) {
  msg = new MyMessage(sensorId, V_TEMP);  
}

void MySensorInternalTemperature::initDevice() {
  send(msg->set((int)sensor->measureT()));
  lastUpdate = millis();
}

void MySensorInternalTemperature::updateDevice() {
  if (millis() > lastUpdate + INTERVAL)  {
    send(msg->set((int)sensor->measureT()));
    lastUpdate = millis();
  }

  
}


/*  *******************************************************************************************
 *                                    MySensor internal temperature
 *  *******************************************************************************************/
MySensorExternalTemperature::MySensorExternalTemperature(const char *description, ExternalTemperature * sensor): MySensorDevice(S_TEMP, description),sensor(sensor) {
  msg = new MyMessage(sensorId, V_TEMP);  
}

void MySensorExternalTemperature::initDevice() {
  sensor->setup();
}

void MySensorExternalTemperature::updateDevice() {
  if (millis() > lastUpdate + INTERVAL)  {
    int error = sensor->readValues();
    if (!error) {
      for (uint8_t i=0; i<sensor->getLastSensorCount(); i++) {
        send(msg->setSensor(sensorId+i).set(sensor->getTemperature(i),1));
      }
      lastUpdate = millis();
    }
  }
}
