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
void(* resetFunc) (void) = 0;
char NAME[20];
Memory nameMemory(20,SHIFT_NODENAME);
Memory configMemory(40,SHIFT_NODECONFIG);
MySensorDevice * mySensorDevices[MAXDEVICESCOUNT];

/*  *******************************************************************************************
                                            Before
 *  *******************************************************************************************/
void before() {

  char config[40];
  nameMemory.loadString(NAME, "VanillaSensor");  //default: VanillaSensor
  configMemory.loadString(config, "41324");      //default: relay1, button1, relay2, button2

  wait(MY_NODE_ID * INIT_DELAY);
    
  MySensorDeviceFactory factory;
  uint8_t deviceCount = (uint8_t)config[0]-48;
  for (uint8_t i = 0; i < deviceCount; i++) {
    mySensorDevices[i] = factory.createDevice(DEVICE(config[i+1]-48));
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
  //      topic/node/child/command/ack/type
  // example: gwi/25/0/1(set)/0/48    0 Unlock| 1 Reset | 2 Name  |  3 Config  | 4 RS 
  bool static lock = true;
  if ((message.sensor == 0) && (message.type == V_CUSTOM)) {
    char * buf = message.getString();
    if (buf[0]=='0') lock = false;
    if (lock) return;
    if (buf[0]=='1') resetFunc();
    else if (buf[0]=='2') nameMemory.saveString(buf+1);
    else if (buf[0]=='3') configMemory.saveString(buf+1);
    else if (buf[0]=='4') Memory(3, SHIFT_RS).save(buf+1,0).save(buf+2,1).save(buf+3,2);
  }
  
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
