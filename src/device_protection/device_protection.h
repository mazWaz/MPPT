#ifndef DEVICE_PROTECTION_H
#define DEVICE_PROTECTION_H

#include "config.h"
#include "./sensor/sensor_reading.h"
#include "./system_process/system_process.h"

class SensorReading;
class SystemProcess;

class DeviceProtection
{
public:
    DeviceProtection(uint8_t backflow_mosfet);
    void setup(SystemProcess *system_process, SensorReading *sensors);
    void deviceProtection();

    int getERR() { return ERR; };
    bool getFLV() { return FLV; };
    bool getBNC() { return BNC; };
    bool getIUV() { return IUV; };
    bool getIOC() { return IOC; };
    bool getOOV() { return OOV; };
    bool getOOC() { return OOC; };
    bool getOTE() { return OTE; };
    bool getREC() { return REC; };
    bool getBypassEnable() { return bypassEnable; };

    void setREC(bool rec)
    {
        REC = rec;
    }

private:
    void backflowControl();

    SystemProcess *systemProcess;
    SensorReading *sensor;

    uint8_t backflowMosfet;
    int
        errorTimeLimit = 1000,   //  USER PARAMETER - Time interval for reseting error counter (milliseconds)
        errorCount = 5,          //  USER PARAMETER - Maximum number of errors
        voltageDropout = 1.0000, //  CALIB PARAMETER - Buck regulator's dropout voltage (DOV is present due to Max Duty Cycle Limit)
        errorCountLimit = 5,     //  USER PARAMETER - Maximum number of errors
        ERR = 0;                 // SYSTEM PARAMETER -
    long
        prevErrorMillis = 0,      // SYSTEM PARAMETER -
        currentRoutineMillis = 0; // SYSTEM PARAMETER -
    unsigned long
        currentErrorMillis = 0; // SYSTEM PARAMETER -
    float
        currentInAbsolute = 31.0000,   //  CALIB PARAMETER - Maximum Input Current The System Can Handle (A - Input)
        currentOutAbsolute = 50.0000,  //  CALIB PARAMETER - Maximum Output Current The System Can Handle (A - Input)
        voltageBatteryThresh = 1.5000, //  CALIB PARAMETER - Power cuts-off when this voltage is reached (Output V)
        vInSystemMin = 10.000;         //  CALIB PARAMETER -
    bool
        bypassEnable = false, // SYSTEM PARAMETER -
        OTE = false,          // OTE - OVERTEMPERATURE: System overheat detected
        IOC = false,          // IOC - INPUT  OVERCURRENT: Input current has reached absolute limit
        OOC = false,          // OOC - OUTPUT OVERCURRENT: Output current has reached absolute limit
        OOV = false,          // OOV - OUTPUT OVERVOLTAGE: Output voltage has reached absolute limit
        FLV = false,          // FLV - Fatally low system voltage (unable to resume operation)
        IUV = false,          // IUV - INPUT UNDERVOLTAGE: Input voltage is below battery voltage (for psu mode only)
        REC = false,          // Clear recovery and battery not connected boolean identifiers
        BNC = false;          // BNC - BATTERY NOT CONNECTED (for charger mode only, does not treat BNC as error when not under MPPT mode)
};

#endif