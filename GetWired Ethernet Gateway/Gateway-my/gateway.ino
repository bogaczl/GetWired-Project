#define MY_RS485                                          // Enable RS485 transport layer
#define MY_RS485_DE_PIN 7                                 // Define this to enables DE-pin management on defined pin
#define MY_RS485_BAUD_RATE 57600                          // Set RS485 baud rate to use
#define MY_RS485_HWSERIAL Serial                          // Enable for Hardware Serial
#define MY_RS485_SOH_COUNT 3                              // Use this in case of collisions on the bus

#define MY_GATEWAY_ENC28J60                               // Enable gateway ethernet module type 
#define MY_IP_ADDRESS 192,168,0,200                       // Gateway IP address
#define MY_MAC_ADDRESS 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEB // Gateway MAC address

#define MY_GATEWAY_MQTT_CLIENT
#define MY_MQTT_PUBLISH_TOPIC_PREFIX "gwi"
#define MY_MQTT_SUBSCRIBE_TOPIC_PREFIX "gwo"
#define MY_MQTT_CLIENT_ID "GW"

#define MY_CONTROLLER_IP_ADDRESS 192,168,0,175  
#define MY_PORT 1883                                      // The port to keep open on node server mode / or port to contact in client mode
#define MY_MQTT_USER "homeassistant"
#define MY_MQTT_PASSWORD "woh2xeezooquahfiN3yahjoo1eesheiW0meophaideeT6eiwail1ohquoo3aeph2"

#define MY_WITH_LEDS_BLINKING_INVERSE                     // Define to inverse LED behaviour
#define MY_DEFAULT_LED_BLINK_PERIOD 10                    // Set blinking period
#define MY_DEFAULT_ERR_LED_PIN 4                          // Error led pin
#define MY_DEFAULT_RX_LED_PIN  5                          // Receive led pin
#define MY_DEFAULT_TX_LED_PIN  6                          // Transmit led pin

//#define MY_DEBUG                                        // Enable debug prints to serial monitor

//#define ENABLE_WATCHDOG                                   // Resets the Gateway in case of any software hang
#define ENABLE_UPLINK_CHECK                               // Resets the Gateway in case of connection loss with the controller
#define UPLINK_CHECK_INTERVAL 60000                       // Time interval for the uplink check (default 60000)

// Includes
#include <UIPEthernet.h>
#include <MySensors.h>

// Definitions
#define CONF_BUTTON A0
#define CONF_LED 9
#define BUS_RELAY 8

// Globals
bool ButtonState = false;
bool ButtonHigh = false;
bool CheckControllerUplink = true;
uint32_t LastUpdate = 0;

void before() {

  #ifdef ENABLE_WATCHDOG
    wdt_reset();
    MCUSR = 0;
    wdt_disable();
  #endif
  
  pinMode(CONF_BUTTON, INPUT_PULLUP);
  pinMode(BUS_RELAY, OUTPUT);
  pinMode(CONF_LED, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

void setup()  {

  #ifdef ENABLE_WATCHDOG
    wdt_enable(WDTO_4S);
  #endif  
}

void loop() {

  wait(500);

  #ifdef ENABLE_UPLINK_CHECK
    if((millis() > LastUpdate + UPLINK_CHECK_INTERVAL) && CheckControllerUplink) {
      if(!requestTime())  {
        delay(10000);
      }
      LastUpdate = millis();
    }
  #endif

  if(!ButtonHigh)  {
    if(digitalRead(CONF_BUTTON))  {
      ButtonHigh = true;
    }
  }

  if(!digitalRead(CONF_BUTTON) && ButtonHigh) {
    uint32_t TIME_1 = millis();
    ButtonHigh = false;
    while(!digitalRead(CONF_BUTTON)) {
      wait(251);
      if(millis() - TIME_1 > 500) {
        ButtonState = !ButtonState;
        
        digitalWrite(BUS_RELAY, ButtonState);
        digitalWrite(CONF_LED, ButtonState);
        
        CheckControllerUplink = (ButtonState != 1);
       
        break;
      }
    }
  }
}
