#ifndef MySensordevice_h
#define MySensordevice_h


enum class DEVICE { 
  Relay1 = 1,
  Relay2 = 2,
  Button1 = 3,
  Button2 = 4,
  Input1 = 5,
  Input2 = 6,
  RelayButton1 = 7,
  RelayButton2 = 8,
  Shutter = 9,
  PowerSensor = 10,
  InternTemp = 11,
  ExternTempDS = 12,
  ExternTempDHT = 13,
  ExternTempSHT = 14
};

/*  *******************************************************************************************
 *                                    MySensor device decorators
 *  *******************************************************************************************/
class MySensorDevice {
    const mysensors_sensor_t sensorType;
    static int usedIDs;
  protected:
    const char *description;
    const uint8_t sensorId;
  public:
    MySensorDevice(const mysensors_sensor_t sensorType, const char *description="", int size=1);
    virtual void presentDevice();
    virtual void initDevice() = 0;
    virtual void updateDevice() = 0;
    virtual void processMessage(const MyMessage &message){ };
};

class MySensorSimpleInput : public MySensorDevice { 
    MyMessage * msg;
    Input * input;
    uint8_t previousState;
  public:
    MySensorSimpleInput(const char *description, Input * input);
    virtual void initDevice();
    virtual void updateDevice();
};

class MySensorButton : public MySensorDevice {
  protected:  
    MyMessage * msgShort;
    MyMessage * msgLong;
    MyMessage * msgDouble;
    Button * button;
    uint8_t previousLongState;
  public:
    MySensorButton(const char *description, Button * button);
    virtual void presentDevice();
    virtual void initDevice();
    virtual void updateDevice();
};

class MySensorRelay : public MySensorDevice {
    Relay * relay;
    uint8_t previousState;
    MyMessage * msg;
  public:
    MySensorRelay(const char *description, Relay * relay);
    virtual void initDevice();
    virtual void updateDevice();
    virtual void processMessage(const MyMessage &message);
};

class MySensorRelayButton : public MySensorButton {
    Relay *relay;
  public:
    MySensorRelayButton(const char *description, Button * button, Relay * relay);
    virtual void presentDevice();
    virtual void initDevice();
    virtual void updateDevice();
};

class MySensorPowerSensor : public MySensorDevice {
    PowerSensor * powerSensor;
    Relay * relay1;
    Relay * relay2;
    float previousValue;
    bool fuseBroken = false;
    MyMessage * msg;
    MyMessage * fuse;
  public:
    MySensorPowerSensor(const char *description, PowerSensor * sensor, Relay * relay1, Relay * relay2);
    virtual void presentDevice();
    virtual void initDevice();
    virtual void updateDevice();
    virtual void processMessage(const MyMessage &);
};

class MySensorInternalTemperature : public MySensorDevice {
    InternalTemperature * sensor;
    Relay * relay1;
    Relay * relay2;
    bool fuseBroken = false;
    MyMessage * msg;
    MyMessage * fuse;
    unsigned long lastUpdate;
  public:
    MySensorInternalTemperature(const char *description, InternalTemperature * sensor, Relay * relay1, Relay * relay2);
    virtual void presentDevice();
    virtual void initDevice();
    virtual void updateDevice();
    virtual void processMessage(const MyMessage &);
};

class MySensorExternalTemperature : public MySensorDevice {
    ExternalTemperature * sensor;
    MyMessage * msg;
    unsigned long lastUpdate;    
  public:
    MySensorExternalTemperature(const char *description, ExternalTemperature * sensor);
    virtual void initDevice();
    virtual void updateDevice();
};

class MySensorShutterControler : public MySensorDevice {
    const unsigned long CALIBRATION_DELAY;
    Button * upButton;
    Button * downButton;
    SmartShutterControler * smartShutterControler;
    PowerSensor * powerSensor;
    Memory * memory;
    MyMessage * msgPercent;
    MyMessage * msgDoubleClick;
    uint8_t desiredPosition;
    uint8_t position;
    void goUp();
    void goDown();
    void stop();
    int calibrateUpTime();
    int calibrateDownTime();
    void calibrate();
  public:    
    MySensorShutterControler(const char *description, Button * upButton, Button * downButton, ShutterControler * shutterControler, PowerSensor * sensor, Memory * memory);
    virtual void presentDevice();
    virtual void initDevice();
    virtual void processMessage(const MyMessage &);
    virtual void updateDevice();
};


class MySensorDeviceFactory {
  Relay * relay1;
  Relay * relay2;
  PowerSensor * powerSensor;
public:
  MySensorDeviceFactory();
  MySensorDevice * createDevice(DEVICE device);
};


#endif
