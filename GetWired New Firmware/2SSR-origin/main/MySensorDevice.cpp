#include "Arduino.h"
#include <core/MySensorsCore.h>
#include "Memory.h"
#include "Device.h"
#include "PowerSensor.h"
#include "InternalTemperature.h"
#include "ExternalTemperature.h"
#include "MySensorDevice.h"

MySensorDeviceFactory::MySensorDeviceFactory() {
  relay1 = new Relay( RELAY_1);
  relay2 = new Relay( RELAY_2);
  powerSensor = new PowerSensor( PS_PIN );
}

MySensorDevice * MySensorDeviceFactory::createDevice(DEVICE device) {
  switch (device) {
    case DEVICE::Relay1:        return new MySensorRelay("Relay 1", relay1 );
    case DEVICE::Relay2:        return new MySensorRelay("Relay 2", relay2 ); 
    case DEVICE::Button1:       return new MySensorButton("Button 1", new Button( BUTTON_1 )); 
    case DEVICE::Button2:       return new MySensorButton("Button 2", new Button( BUTTON_2 )); 
    case DEVICE::Input1:        return new MySensorSimpleInput("Input 1", new Input( INPUT_PIN_3 )); 
    case DEVICE::Input2:        return new MySensorSimpleInput("Input 2", new Input( INPUT_PIN_4 )); 
    case DEVICE::RelayButton1:  return new MySensorRelayButton("Button 1", new Button( BUTTON_1 ), relay1 ); 
    case DEVICE::RelayButton2:  return new MySensorRelayButton("Button 2", new Button( BUTTON_2 ), relay2 ); 
    case DEVICE::Shutter:       return new MySensorShutterControler("Shutter", new Button( BUTTON_1 ), new Button( BUTTON_2 ), new ShutterControler(RELAY_1, RELAY_2), powerSensor, new Memory(3, SHIFT_RS));
    case DEVICE::PowerSensor:   return new MySensorPowerSensor("Power Sensor", powerSensor, relay1, relay2);
    case DEVICE::InternTemp:    return new MySensorInternalTemperature("Internal Thermometer", new InternalTemperature( IT_PIN ), relay1, relay2);
    case DEVICE::ExternTempDS:  return new MySensorExternalTemperature("External Thermometer", new ExternalTemperature_DS18B20( ONE_WIRE_PIN ));
    case DEVICE::ExternTempDHT: return new MySensorExternalTemperature("External Thermometer", new ExternalTemperature_DHT22( ONE_WIRE_PIN ));
    case DEVICE::ExternTempSHT: return new MySensorExternalTemperature("External Thermometer", new ExternalTemperature_SHT30( ));
  }
  return NULL;
}

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
MySensorButton::MySensorButton(const char *description, Button * button):MySensorDevice(S_BINARY, description, 3),button(button) { 
    msgShort = new MyMessage(sensorId,V_LIGHT);
    msgLong = new MyMessage(sensorId+1,V_LIGHT);
    msgDouble = new MyMessage(sensorId+2,V_LIGHT);
}

void MySensorButton::presentDevice() {
  char con[40];
  sprintf(con,"%s %s",description, "short");  
  present(sensorId, S_BINARY, con);
  wait(PRESENTATION_DELAY);
  sprintf(con,"%s %s",description, "long");  
  present(sensorId+1, S_BINARY, con);
  wait(PRESENTATION_DELAY);   
  sprintf(con,"%s %s",description, "double");  
  present(sensorId+2, S_BINARY, con);
  wait(PRESENTATION_DELAY);   
}

void MySensorButton::initDevice() {
  send(msgShort->setSensor(sensorId).set(button->getShortState()));
  previousLongState = button->getLongState();
  send(msgLong->setSensor(sensorId+1).set(previousLongState));
  send(msgDouble->setSensor(sensorId+2).set(0));
}

void MySensorButton::updateDevice() {
  button->checkInput();
  uint8_t state = button->getShortState();
  if (state) {
    send(msgShort->set("1"));
    delay(50);
    send(msgShort->set("0"));
  }
  state = button->getLongState();
  if (previousLongState != state) {
    send(msgLong->set(state));
    previousLongState = state;
  }
  state = button->getDoubleState();
  if (state) {
    send(msgDouble->set("1"));
    delay(50);
    send(msgDouble->set("0"));
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
  if (message.sensor != sensorId) {
    return;
  }
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
 *                                    MySensor external temperature
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


/*  *******************************************************************************************
 *                                    MySensor shutter controler
 *  *******************************************************************************************/
MySensorShutterControler::MySensorShutterControler(const char *description, Button * upButton, Button * downButton, ShutterControler * shutterControler, PowerSensor * sensor, Memory * memory):MySensorDevice(S_COVER, description, 3),upButton(upButton),downButton(downButton),powerSensor(sensor),memory(memory),CALIBRATION_DELAY(10000),position(0) {
  
  msgPercent = new MyMessage (sensorId, V_PERCENTAGE);
  msgUpDoubleClick = new MyMessage(sensorId+1, V_LIGHT);
  msgDownDoubleClick = new MyMessage(sensorId+2, V_LIGHT);
  smartShutterControler = new SmartShutterControler(shutterControler);
  position = memory->load(0);
  if (position>100) {
    position=100;
  }
  if ((memory->load(1) != 255) && (memory->load(2) != 255)) {
    smartShutterControler->calibrate(memory->load(1), memory->load(2), position);
  } 
}

void MySensorShutterControler::goUp() {
  desiredPosition=100;
  smartShutterControler->goUp();
}

void MySensorShutterControler::goDown() {
  desiredPosition=0;
  smartShutterControler->goDown();
}

void MySensorShutterControler::stop() {
  desiredPosition=smartShutterControler->getPosition();
  smartShutterControler->stop();
}

int MySensorShutterControler::calibrateUpTime() {
  unsigned long startTime = millis();
  smartShutterControler->goUp();
  delay(2000);
  float workCurrent = powerSensor->measureAC();
  while(powerSensor->measureAC() > 0.25*workCurrent) {
    if (millis()-startTime > 120000) {
      return 0;
    }
    delay(50);
  }
  smartShutterControler->stop();
  return (millis()-startTime)/1000;
}

int MySensorShutterControler::calibrateDownTime() {
  unsigned long startTime = millis();
  smartShutterControler->goDown();
  delay(2000);
  float workCurrent = powerSensor->measureAC();
  while(powerSensor->measureAC() > 0.25*workCurrent) {
    if (millis()-startTime > 120000) {
      return 0;
    }
    delay(50);
  }
  smartShutterControler->stop();
  return (millis()-startTime)/1000;
}

void MySensorShutterControler::calibrate() {
  float workCurrent;
  int position = smartShutterControler->getPosition();
  unsigned long upTime, downTime;
  if (position == 0) {
    upTime = calibrateUpTime();
    downTime = calibrateDownTime();
  }
  if (position == 100) {
    downTime = calibrateDownTime();
    upTime = calibrateUpTime();
    
  }
  if (upTime > 255) {
    upTime = 254;
  }
  if (downTime > 255) {
    downTime = 254;
  }
  smartShutterControler->calibrate(upTime, downTime, position);
  memory->save(position, 0);
  memory->save(upTime, 1);
  memory->save(downTime, 2);
}

void MySensorShutterControler::initDevice() {
  send(MyMessage(sensorId, V_UP).set(0));
  request(sensorId, V_UP);
  wait(2000, C_SET, V_UP);

  send(MyMessage(sensorId, V_DOWN).set(0));
  request(sensorId, V_DOWN);
  wait(2000, C_SET, V_DOWN);

  send(MyMessage(sensorId, V_STOP).set(1));
  request(sensorId, V_STOP);
  wait(2000, C_SET, V_STOP);

  send(msgPercent->setSensor(sensorId).set(position));
  request(sensorId, V_PERCENTAGE);
  wait(2000, C_SET, V_PERCENTAGE);

  send(msgUpDoubleClick->setSensor(sensorId+1).set(0));
  request(sensorId+1, V_STATUS);
  wait(2000, C_SET, V_STATUS);

  send(msgDownDoubleClick->setSensor(sensorId+2).set(0));
  request(sensorId+2, V_STATUS);
  wait(2000, C_SET, V_STATUS);
}

void MySensorShutterControler::presentDevice() {  
  MySensorDevice::presentDevice();
  char con[40];
  sprintf(con,"%s %s",description, "up double");  
  present(sensorId+1, S_BINARY, con);
  wait(PRESENTATION_DELAY);        

  sprintf(con,"%s %s",description, "down double");  
  present(sensorId+2, S_BINARY, con);
  wait(PRESENTATION_DELAY);        
}

void MySensorShutterControler::processMessage(const MyMessage &message) {
  if (message.sensor != sensorId) {
    return;
  }
  if (message.type == V_PERCENTAGE) {
    if (!smartShutterControler->isCalibrated()) {
      return;
    }
    desiredPosition = atoi(message.data);
    if (desiredPosition > 100) {
      desiredPosition = 100;
    }
    if (desiredPosition < 0) {
      desiredPosition = 0;
    }
    if (desiredPosition > smartShutterControler->getPosition()) {
      smartShutterControler->goUp();
    }
    else if (desiredPosition < smartShutterControler->getPosition()) {
      smartShutterControler->goDown();
    }
  }
  else if(message.type == V_UP) {
    goUp();
  }
  else if(message.type == V_DOWN) {
    goDown();
  }
  else if(message.type == V_STOP) {
    stop();
  }
}

// logic:
// press 
//  - if stopped -> it starts:
//    - release before 1s -> goes to edge (if calibrated)
//    - release after 1s -> stops 
//  - if runes -> stops
// calibrate
//  - long press (>10s), when on edge 

void MySensorShutterControler::updateDevice() {
  static int previousPosition = 0;
  static SmartShutterControler::State previousState = SmartShutterControler::State::IDLE;
  position = smartShutterControler->getPosition();
  SmartShutterControler::State state = smartShutterControler->getState();
 
  upButton->checkInput();
  if (upButton->getShortState()) {
    if (state==SmartShutterControler::State::IDLE) {
      goUp();
    }
    else {
      stop();
    }
    state = smartShutterControler->getState();
  }
  if (upButton->getLongState()) {
    if ((position == 100) && (upButton->getLongPressTime() > CALIBRATION_DELAY)) {
      calibrate();
      return;
    }
    smartShutterControler->goUp();
    desiredPosition=smartShutterControler->getPosition();
  }
  if (upButton->getDoubleState()) {
      send(msgUpDoubleClick->set(1));
      delay(100);
      send(msgUpDoubleClick->set(0));
  }
  
  downButton->checkInput();
  if (downButton->getShortState()) {
    if (state==SmartShutterControler::State::IDLE) {
      goDown();
    }
    else {
      stop();
    }
    state = smartShutterControler->getState();
  }
  if (downButton->getLongState()) {
    if ((position == 0) && (downButton->getLongPressTime() > CALIBRATION_DELAY)) {
      calibrate();
      return;
    }
    smartShutterControler->goDown();
    desiredPosition = smartShutterControler->getPosition();
  }
  if (downButton->getDoubleState()) {
      send(msgDownDoubleClick->set(1));
      delay(100);
      send(msgDownDoubleClick->set(0));
  }

  if ((!upButton->getLongState()) && (!downButton->getLongState()))
    if (state!=SmartShutterControler::State::IDLE) {  
      if (((state==SmartShutterControler::State::GOING_UP) && (position >= desiredPosition)) ||
          ((state==SmartShutterControler::State::GOING_DOWN) && (position <= desiredPosition)))
    {
        stop();
    }
  }
  if (position != previousPosition) {
    previousPosition = position;
    send(msgPercent->set(position));
  }
  if (state != previousState) {
    previousState = state;
    send(MyMessage(sensorId, V_UP).set(state == SmartShutterControler::State::GOING_UP));
    send(MyMessage(sensorId, V_DOWN).set(state == SmartShutterControler::State::GOING_DOWN));
    send(MyMessage(sensorId, V_STOP).set(state == SmartShutterControler::State::IDLE));
    if (state == SmartShutterControler::State::IDLE) {
      memory->save(position);
    }
  }
}
