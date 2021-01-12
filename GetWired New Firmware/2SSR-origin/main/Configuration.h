#ifndef Configuration_h
#define Configuration_h

/*  *******************************************************************************************
                    MySensors Definitions
 *  *******************************************************************************************/
// Identification
#define MY_NODE_ID 5                           // Set node ID
#define SN "GetWired-5-DR"                     // Set node name to present to a controller
#define SV "1.0"                               // Set sensor version

// Selecting transmission settings
#define MY_DEBUG
#define MY_RS485                              // Enable RS485 transport layer
#define MY_RS485_DE_PIN 7                     // DE Pin definition
#define MY_RS485_BAUD_RATE 57600              // Set RS485 baud rate
#define MY_RS485_HWSERIAL Serial              // Enable Hardware Serial
#define MY_RS485_SOH_COUNT 3                  // Collision avoidance

// FOTA Feature
#define MY_OTA_FIRMWARE_FEATURE                 // Enable OTA feature

// Other
#define MY_TRANSPORT_WAIT_READY_MS 60000        // Time to wait for gateway to respond at startup (default 60000)
#define MAXDEVICESCOUNT 20

// Output Config - one of these has to be defined
#define DOUBLE_RELAY              // Define this node as a double relay node, setting below
//#define ROLLER_SHUTTER            // Define this node as a roller shutter node, setting below
//#define FOUR_RELAY                // Define this node as a four relay node, setting below
//#define DIMMER                    // Define this node as a 1-channel dimmer node, setting below
//#define RGB                       // Define this node as a RGB dimmer node, setting below
//#define RGBW                      // Define this node as a RGBW dimmer node, setting below

/*  *******************************************************************************************
                    MCU Pin Definitions
 *  *******************************************************************************************/
// OUTPUT [RELAY / RGBW]
#define OUTPUT_PIN_1 5
#define OUTPUT_PIN_2 9
#define OUTPUT_PIN_3 6
#define OUTPUT_PIN_4 10

// INPUT [BUTTON / SENSOR]
// General input
#define INPUT_PIN_1 2             // default 2
#define INPUT_PIN_2 3             // default 3
#define INPUT_PIN_3 4             // default 4
#define INPUT_PIN_4 A3            // default A3

// Analog input
#define INPUT_PIN_5 A1            // default A1
#define INPUT_PIN_6 A2            // default A2
#define INPUT_PIN_7 A6            // default A6
#define INPUT_PIN_8 A7            // default A7

// Protocols
// 1-wire
#define ONE_WIRE_PIN A0           // default A0

// I2C
#define I2C_PIN_1 A4              // default A4
#define I2C_PIN_2 A5              // default A5

/*  *******************************************************************************************
                    Various Definitions
 *  *******************************************************************************************/
// General
#define RELAY_ON HIGH                       // Pin state to turn the relays on (default HIGH)
#define RELAY_OFF LOW                       // Pin state to turn the relays off (default LOW)

// Internal temperature sensor
#define MVPERC 10                           // V per 1 degree celsius (default 10)
#define ZEROVOLTAGE 500                     // Voltage output of temperature sensor (default 500)
#define MAX_TEMPERATURE 85                  // Maximum temperature the module can have before reporting error (default 85)

// Power Sensor
#define MAX_CURRENT 3                       // Maximum current the module can handle before reporting error (2SSR - 3; 4RelayDin - 10A or 16)
#define POWER_MEASURING_TIME 20             // Current measuring takes this long (default 20)
#define MVPERAMP 185                        // mV per 1A (default: 2SSR 185 mV/A; 4RelayDin 73.3 mV/A, RGBW 100 mV/A)
#define RECEIVER_VOLTAGE 230                // 230V, 24V, 12V - values for power usage calculation, depends on the receiver
#define COSFI 1                             // cos(fi) value for a given load: resistive load - 1, LED - 0.4 < cos(fi) < 0.99, fluorescent - 

// Other
#define INTERVAL 300000                     // Interval value for reporting readings of the sensors: temperature, power usage (default 300000)
#define INIT_DELAY 200                      // A value to be multiplied by node ID value to obtain the time to wait during the initialization process
#define PRESENTATION_DELAY 10               // Time (ms) to wait between subsequent presentation messages (default 10)
#define LOOP_TIME 100                       // Main loop wait time (default 100)        

/*  *******************************************************************************************
                    OUTPUT Config
 *  *******************************************************************************************/
// 2SSR DOUBLE_RELAY
#ifdef DOUBLE_RELAY
  #define RELAY_ID_1 0
  #define RELAY_ID_2 1
  #define RELAY_1 OUTPUT_PIN_1
  #define RELAY_2 OUTPUT_PIN_2
  #define BUTTON_1 INPUT_PIN_1
  #define BUTTON_2 INPUT_PIN_2
  #define NUMBER_OF_RELAYS 2
#endif

// Roller Shutter
#ifdef ROLLER_SHUTTER
  #define RS_ID 0
  #define RELAY_ID_1 0
  #define RELAY_1 OUTPUT_PIN_1
  #define RELAY_2 OUTPUT_PIN_2
  #define BUTTON_1 INPUT_PIN_1
  #define BUTTON_2 INPUT_PIN_2
  #define NUMBER_OF_RELAYS 2
#endif

#define PS_PIN INPUT_PIN_7

#if defined(DOUBLE_RELAY) || defined(ROLLER_SHUTTER)
  #define IT_PIN INPUT_PIN_8
#elif defined(DIMMER) || defined(RGB) || defined(RGBW)
  #define IT_PIN INPUT_PIN_7
#endif

/*  *******************************************************************************************
                    EEPROM Definitions
 *  *******************************************************************************************/
#define SIZE_OF_INT 2
#define EEPROM_OFFSET 512                               // First eeprom address to use (prior addresses are taken)
#define EEA_RS_TIME_DOWN EEPROM_OFFSET                  // EEPROM address to save RShutter travel down time
#define EEA_RS_TIME_UP EEA_RS_TIME_DOWN+SIZE_OF_INT     // EEPROM address to save RShutter travel up time
#define EEA_RS_POSITION EEA_RS_TIME_UP+SIZE_OF_INT      // EEPROM address to save RShutter last known position

#endif
