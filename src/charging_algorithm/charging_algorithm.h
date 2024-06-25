#ifndef CHARING_ALGORITHM_H
#define CHARING_ALGORITHM_H

#include "config.h"
#include "./sensor/sensor_reading.h"
#include "./device_protection/device_protection.h"
#include "./system_process/system_process.h"

class SensorReading;
class DeviceProtection;
class SystemProcess;

class ChargingAlgorithm
{

public:
    ChargingAlgorithm(uint8_t buck_en, uint8_t buck_in);
    void setup(SystemProcess *system_process, SensorReading *sensors, DeviceProtection *device_protection);
    void chargingAlgorithm();

    int getPwm() { return PWM; };
    int getPpwm() { return PPWM; };
    int getPwmMax() { return pwmMax; };
    int getPwmMaxLimited() { return pwmMaxLimited; };
    unsigned long getChargingLoopTimme() { return chargingLoopTimme; };
    bool getBuckState() { return buckState; };

private:
    void buckEnable();
    void buckDisable();
    void predictedPWM(float voltage_input, float voltage_batt);
    void modulationPWM(bool output_mode, float voltage_input, float voltage_batt);

    SystemProcess *systemProcess;
    SensorReading *sensor;
    DeviceProtection *deviceProtection;
    uint8_t buckEN,
        buckIN;
    unsigned long
        chargingLoopTimme = 0, // SYSTEM PARAMETER -
        loopTimeStart = 0,     // SYSTEM PARAMETER - Used for the loop cycle stop watch, records the loop start time
        loopTimeEnd = 0;       // SYSTEM PARAMETER - Used for the loop cycle stop watch, records the loop end time
    int
        PWM = 0,           // SYSTEM PARAMETER -
        PPWM = 0,          // SYSTEM PARAMETER -
        pwmMax = 0,        // SYSTEM PARAMETER -
        pwmMaxLimited = 0; // SYSTEM PARAMETER -

    float
        PPWMargin = 99.5000,       //  CALIB PARAMETER - Minimum Operating Duty Cycle for Predictive PWM (%)
        powerInputPrev = 0.0000,   // SYSTEM PARAMETER - Previously stored input power variable for MPPT algorithm (Watts)
        voltageInputPrev = 0.0000; // SYSTEM PARAMETER - Previously stored input voltage variable for MPPT algorithm
    bool
        chargingPause = 0, // SYSTEM PARAMETER -
        buckState = false; // SYSTEM PARAMETER - Buck Enable Status
};

#endif