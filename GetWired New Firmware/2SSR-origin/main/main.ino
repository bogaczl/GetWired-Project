/*  *******************************************************************************************
                                        Includes
 *  *******************************************************************************************/
#include "Configuration.h"
#include <MySensors.h>
#include "Memory.h"
#include "Device.h"
#include "PowerSensor.h"
#include "InternalTemperature.h"
#include "ExternalTemperature.h"
#include "MySensorDevice.h"

/*  *******************************************************************************************
                                        Globals
 *  *******************************************************************************************/
// Initialization
char * NAME = "VanillaSensor";
MySensorDevice * mySensorDevices[MAXDEVICESCOUNT];

/*  *******************************************************************************************
                                            Before
 *  *******************************************************************************************/
void before() {
  
  uint32_t InitDelay = MY_NODE_ID * INIT_DELAY;
  wait(InitDelay);

  Memory * config = new Memory(100);
  if (config->load() != 255) {

  }
  else {
    int i=0;
    MySensorDeviceFactory factory;
    mySensorDevices[i++] = factory.createDevice(DEVICE::Relay1);
    mySensorDevices[i++] = factory.createDevice(DEVICE::Button1);
    mySensorDevices[i++] = factory.createDevice(DEVICE::Relay2);
    mySensorDevices[i++] = factory.createDevice(DEVICE::Button2);
    mySensorDevices[i++] = factory.createDevice(DEVICE::PowerSensor);
    mySensorDevices[i++] = factory.createDevice(DEVICE::InternTemp);
    mySensorDevices[i++] = factory.createDevice(DEVICE::ExternTempDHT);
  }
}

/*  *******************************************************************************************
                                            Presentation
 *  *******************************************************************************************/
void presentation() {

  sendSketchInfo(NAME, "1.0");
 
  for ( MySensorDevice * d : mySensorDevices) {
    if (d)
      d->presentDevice();
  }
}

/*  *******************************************************************************************
                                            Init Confirmation
 *  *******************************************************************************************/
void InitConfirmation() {

  for ( MySensorDevice * d : mySensorDevices) {
    if (d) 
      d->initDevice();
  }
}


/*  *******************************************************************************************
                                        MySensors Receive
 *  *******************************************************************************************/
void receive(const MyMessage &message)  {
   for ( MySensorDevice * d : mySensorDevices) {
    if (d)
      d->processMessage(message);
  }
}

/*  *******************************************************************************************
                                        Main Loop
 *  *******************************************************************************************/
void loop() {
  static bool InitConfirm = false;
  if (!InitConfirm)  {
    InitConfirmation();
    InitConfirm = true;
  }

  for ( MySensorDevice * d : mySensorDevices) {
    if (d)
      d->updateDevice();
  }

  wait(LOOP_TIME);
}
