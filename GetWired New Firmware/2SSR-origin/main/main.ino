/*  *******************************************************************************************
                                        Includes
 *  *******************************************************************************************/
#include "Configuration.h"
#include <MySensors.h>
#include "Device.h"
#include "PowerSensor.h"
#include "InternalTemperature.h"
#include "ExternalTemperature.h"
#include "RShutterControl.h"
#include "MySensorDevice.h"

/*  *******************************************************************************************
                                        Globals
 *  *******************************************************************************************/
// Initialization
MySensorDevice * mySensorDevices[MAXDEVICESCOUNT];

/*  *******************************************************************************************
                                            Before
 *  *******************************************************************************************/
void before() {
  
  uint32_t InitDelay = MY_NODE_ID * INIT_DELAY;
  wait(InitDelay);

  int i=0;
  Relay * relay1 = new Relay( RELAY_1);
  mySensorDevices[i++] = new MySensorRelay("Relay 1", relay1 );
  mySensorDevices[i++] = new MySensorRelayButton("Button 1", new Button( BUTTON_1 ), relay1 );
  Relay * relay2 = new Relay( RELAY_2);
  mySensorDevices[i++] = new MySensorRelay("Relay 2", relay2 );
  mySensorDevices[i++] = new MySensorRelayButton("Button 2", new Button( BUTTON_2 ), relay2 ); 
  mySensorDevices[i++] = new MySensorPowerSensor("Power Sensor", new PowerSensor( PS_PIN ), relay1, relay2);
  mySensorDevices[i++] = new MySensorInternalTemperature("Internal Thermometer", new InternalTemperature( IT_PIN ), relay1, relay2);
  mySensorDevices[i++] = new MySensorExternalTemperature("External Thermometer", new ExternalTemperature_DHT22( ONE_WIRE_PIN ));  //DHT22, SHT30, DS18B20
}

/*  *******************************************************************************************
                                            Presentation
 *  *******************************************************************************************/
void presentation() {

  sendSketchInfo(SN, SV);
 
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
