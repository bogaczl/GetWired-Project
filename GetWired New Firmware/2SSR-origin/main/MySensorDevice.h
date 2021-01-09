#ifndef MySensordevice_h
#define MySensordevice_h

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
    virtual void initDevice();
    virtual void updateDevice();
    virtual void processMessage(const MyMessage &message);
};

class MySensorButton : public MySensorDevice {
  protected:  
    MyMessage * msgShort;
    MyMessage * msgLong;
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
    virtual void presentDevice();
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
    MyMessage * msg;
  public:
    MySensorPowerSensor(const char *description, PowerSensor * sensor, Relay * relay1, Relay * relay2);
    virtual void initDevice();
    virtual void updateDevice();
};

class MySensorInternalTemperature : public MySensorDevice {
    InternalTemperature * sensor;
    Relay * relay1;
    Relay * relay2;
    MyMessage * msg;
    unsigned long lastUpdate;
  public:
    MySensorInternalTemperature(const char *description, InternalTemperature * sensor, Relay * relay1, Relay * relay2);
    virtual void initDevice();
    virtual void updateDevice();
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

#endif
