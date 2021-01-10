#include "Arduino.h"
#include <core/MySensorsCore.h>
#include "Device.h"
#include "PowerSensor.h"
#include "InternalTemperature.h"
#include "ExternalTemperature.h"
#include "MySensorDevice.h"

/*  *******************************************************************************************
 *                                    MySensor general device
 *  *******************************************************************************************/
int MySensorDevice::usedIDs = 0;
MySensorDevice::MySensorDevice(const mysensors_sensor_t sensorType, const char *description, int size):sensorId(usedIDs),sensorType(sensorType),description(description) {
  usedIDs += size;
}

void MySensorDevice::presentDevice() {  
  present(sensorId, sensorType, description);
  wait(PRESENTATION_DELAY);   
}

/*  *******************************************************************************************
 *                                    MySensor relay
 *  *******************************************************************************************/
MySensorSimpleInput::MySensorSimpleInput(const char *description, Input * input):MySensorDevice(S_BINARY, description),input(input) {
  msg = new MyMessage(sensorId, V_STATUS);
}

void MySensorSimpleInput::initDevice() {
  previousState = input->getState();
  send(msg->setSensor(sensorId).set(previousState));
  request(sensorId, V_STATUS);
  wait(2000, C_SET, V_STATUS);
}

void MySensorSimpleInput::updateDevice() {
  int state = input->getState();
  if (previousState != state) {
    send(msg->setSensor(sensorId).set(state));
    previousState = state;
  }
}


/*  *******************************************************************************************
 *                                    MySensor button
 *  *******************************************************************************************/
MySensorButton::MySensorButton(const char *description, Button * button):MySensorDevice(S_BINARY, description, 2),button(button) { 
    msgShort = new MyMessage(sensorId,V_LIGHT);
    msgLong = new MyMessage(sensorId+1,V_LIGHT);
}

void MySensorButton::presentDevice() {
  char con[40];
  sprintf(con,"%s %s",description, "short");  
  present(sensorId, S_BINARY, con);
  wait(PRESENTATION_DELAY);
  sprintf(con,"%s %s",description, "long");  
  present(sensorId+1, S_BINARY, con);
  wait(PRESENTATION_DELAY);   
}

void MySensorButton::initDevice() {
  send(msgShort->setSensor(sensorId).set(button->getShortState()));
  previousLongState = button->getLongState();
  send(msgLong->setSensor(sensorId+1).set(previousLongState));
}

void MySensorButton::updateDevice() {
  button->checkInput();
  uint8_t state = button->getShortState();
  if (state) {
    send(msgShort->setSensor(sensorId).set("1"));
    delay(100);
    send(msgShort->setSensor(sensorId).set("0"));
  }
  state = button->getLongState();
  if (previousLongState != state) {
    send(msgLong->setSensor(sensorId+1).set(state));
    previousLongState = state;
  }
}

/*  *******************************************************************************************
 *                                    MySensor relay
 *  *******************************************************************************************/
MySensorRelay::MySensorRelay(const char *description, Relay * relay):MySensorDevice(S_BINARY, description),relay(relay) {
  msg = new MyMessage(sensorId, V_LIGHT);
}

void MySensorRelay::initDevice() {
  previousState = relay->getState();
  send(msg->setSensor(sensorId).set(previousState));
  request(sensorId, V_STATUS);
  wait(2000, C_SET, V_STATUS);
}

void MySensorRelay::updateDevice() {
  int state = relay->getState();
  if (previousState != state) {
    send(msg->setSensor(sensorId).set(state));
    previousState = state;
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
  button->checkInput();
  uint8_t state = button->getShortState();
  if (state) {
    relay->switchState();
    send(msgShort->setSensor(sensorId).set("1"));
    delay(100);
    send(msgShort->setSensor(sensorId).set("0"));
  }
  state = button->getLongState();
  if (previousLongState != state) {
    send(msgLong->setSensor(sensorId+1).set(state));
    previousLongState = state;
  }
}


/*  *******************************************************************************************
 *                                    MySensor power sensor
 *  *******************************************************************************************/
MySensorPowerSensor::MySensorPowerSensor(const char *description, PowerSensor * sensor, Relay * relay1, Relay * relay2): MySensorDevice(S_POWER, description, 2),powerSensor(sensor),relay1(relay1),relay2(relay2) {
  msg = new MyMessage(sensorId, V_WATT);
  fuse = new MyMessage(sensorId+1, V_STATUS);
}

void MySensorPowerSensor::presentDevice() {
  MySensorDevice::presentDevice();
  char con[40];
  sprintf(con,"%s %s",description, "overload");  
  present(sensorId+1, S_BINARY, con);
  wait(PRESENTATION_DELAY);   
}

void MySensorPowerSensor::initDevice() {
  previousValue = 0.0;
  send(msg->setSensor(sensorId).set("0.0"));
  send(fuse->setSensor(sensorId+1).set("1"));
}

void MySensorPowerSensor::updateDevice() {
  float current = 0.0;
  if (relay1->getState() == RELAY_ON || relay2->getState() == RELAY_ON)  {
    current = powerSensor->measureAC();
  }
  if ((fabs(previousValue-current) > 0.05) || (fabs(previousValue - current) > (0.1 * previousValue)) ) {
    send(msg->set(powerSensor->calculatePower(current, COSFI), 0));
    previousValue = current;
  }
  if (powerSensor->isOverload()) {
    fuseBroken = true;
    relay1->setState(0);
    relay2->setState(0); 
    send(fuse->set("0"));
  }
  else if (fuseBroken) {
    fuseBroken = false;
    send(fuse->set("1"));  
  }
}

void MySensorPowerSensor::processMessage(const MyMessage &message) {
  if (message.sensor != sensorId+1) return;
  if (message.type == V_STATUS) {
    if (message.getBool()) {
      fuseBroken = false;
    }
  }
}

/*  *******************************************************************************************
 *                                    MySensor internal temperature
 *  *******************************************************************************************/
MySensorInternalTemperature::MySensorInternalTemperature(const char *description, InternalTemperature * sensor, Relay * relay1, Relay * relay2): MySensorDevice(S_TEMP, description, 2),sensor(sensor),relay1(relay1),relay2(relay2) {
  msg = new MyMessage(sensorId, V_TEMP);  
  fuse = new MyMessage(sensorId+1, V_STATUS);
}

void MySensorInternalTemperature::presentDevice() {
  MySensorDevice::presentDevice();
  char con[40];
  sprintf(con,"%s %s",description, "overheat");  
  present(sensorId+1, S_BINARY, con);
  wait(PRESENTATION_DELAY);   
}

void MySensorInternalTemperature::initDevice() {
  send(msg->set((int)sensor->measureT()));
  send(fuse->setSensor(sensorId+1).set("1"));
  lastUpdate = millis();
}

void MySensorInternalTemperature::updateDevice() {
  
  if (millis() > lastUpdate + INTERVAL)  {
    send(msg->set((int)sensor->measureT()));
    lastUpdate = millis();
  
    if (sensor->isOverheat()) {
      fuseBroken = true;
      relay1->setState(0);
      relay2->setState(0); 
      send(fuse->set("0"));
    }
    else if (fuseBroken) {
      fuseBroken = false;
      send(fuse->set("1"));  
    }
  }
}

void MySensorInternalTemperature::processMessage(const MyMessage &message) {
  if (message.sensor != sensorId+1) return;
  if (message.type == V_STATUS) {
    if (message.getBool()) {
      fuseBroken = false;
    }
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
  lastUpdate = millis();
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
