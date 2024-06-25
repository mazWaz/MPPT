#ifndef SENSOR_READING_H
#define SENSOR_READING_H

#include "config.h"
#include <Adafruit_ADS1X15.h>
#include "./sensor/sensor_reading.h"
#include "./system_process/system_process.h"
#include "./device_protection/device_protection.h"
#include "./charging_algorithm/charging_algorithm.h"

class SystemProcess;
class DeviceProtection;
class ChargingAlgorithm;

class SensorReading
{
public:
    SensorReading(uint8_t address_one, uint8_t address_two, uint8_t temp_sensor);
    void setup(SystemProcess *system_process, DeviceProtection *device_protection, ChargingAlgorithm *charging_algorithm);
    void readSensor();

    float getVoltageInput() { return voltageInput; };
    float getVoltageBattery() { return voltageBattery; };
    float getVoltageLoad() { return voltageLoad; };
    float getCurrentInput() { return currentInput; };
    float getCurrentBattery() { return currentBattery; };
    float getCurrentLoad() { return currentLoad; };
    float getCurrentMidPoint() { return currentMidPointiInput; };
    float getPowerInput() { return powerInput; };
    float getPowerBattery() { return powerBattery; };
    float getPowerLoad() { return powerLoad; };
    float getWhInput() { return WhInput; };
    float getKwhInput() { return kWhInput; };
    float getMWhInput() { return MWhInput; };
    float getWhOutput() { return WhOutput; };
    float getKwhOutput() { return kWhOutput; };
    float getMWhOutput() { return MWhOutput; };
    float getCsiConverted() { return csiConverted; };
    float getOutputDeviation() { return outputDeviation; };
    unsigned long getSensorLoopTimme() { return sensorLoopTimme; };
    int getTemperature() { return temperature; };
    int getInputSource() { return inputSource; };
    int getBattPercent() { return batteryPercent; };

private:
#ifdef ADS1015
    Adafruit_ADS1015 adsOne;
    Adafruit_ADS1015 adsTwo;
#endif
#ifdef ADS1115
    Adafruit_ADS1115 adsOne;
    Adafruit_ADS1115 adsTwo;
#endif
    void readVoltageAmpere();
    void sourceDetection();
    void readTemperature();
    void currentSensorCalibration();
    void powerComputation();
    void batteryPercentage();
    void sensorDataComputation();

    SystemProcess *systemProcess;
    DeviceProtection *deviceProtection;
    ChargingAlgorithm *chargingAlgorithm;

    uint8_t addressOne,
        addressTwo,
        tempSensor;

    unsigned long
        currentRoutineMillis = 0, // SYSTEM PARAMETER -
        prevRoutineMillis = 0,    // SYSTEM PARAMETER -
        sensorLoopTimme = 0,      // SYSTEM PARAMETER -
        loopTimeStart = 0,        // SYSTEM PARAMETER - Used for the loop cycle stop watch, records the loop start time
        loopTimeEnd = 0;          // SYSTEM PARAMETER - Used for the loop cycle stop watch, records the loop end time

    float
        voltageInput = 0.0000,            // SYSTEM PARAMETER - Input voltage (solar voltage)
        voltageBattery = 0.0000,          // SYSTEM PARAMETER - Battery voltage (battery voltage)
        voltageLoad = 0.0000,             // SYSTEM PARAMETER - Load voltage (battery voltage)
        currentInput = 0.0000,            // SYSTEM PARAMETER - Input power (battery or charing voltage)
        currentBattery = 0.0000,          // SYSTEM PARAMETER - Battery current (battery or charing current in Amperes)
        currentLoad = 0.0000,             // SYSTEM PARAMETER - Load current (battery or charing current in Amperes)
        currentMidPointiInput = 2.5250,   //  CALIB PARAMETER - Current Sensor Midpoint (V)
        currentMidPointBatt = 2.6250,     //  CALIB PARAMETER - Current Sensor Midpoint (V)
        currentMidPointLoad = 2.6250,     //  CALIB PARAMETER - Current Sensor Midpoint (V)
        powerInput = 0.0000,              // SYSTEM PARAMETER - Input power (solar power) in Watts
        powerBattery = 0.0000,            // SYSTEM PARAMETER - Battery power (battery or charing power in Watts)
        powerLoad = 0.0000,               // SYSTEM PARAMETER - Load power (Load usage power in Watts)
        vsi = 0.0000,                     // SYSTEM PARAMETER - Raw Input voltage sensor ADC voltage
        vsb = 0.0000,                     // SYSTEM PARAMETER - Raw Battery voltage sensor ADC voltage
        vsl = 0.0000,                     // SYSTEM PARAMETER - Raw Load voltage sensor ADC voltage
        csi = 0.0000,                     // SYSTEM PARAMETER - Raw Input current sensor ADC voltages
        csb = 0.0000,                     // SYSTEM PARAMETER - Raw Battery current sensor ADC voltages
        csl = 0.0000,                     // SYSTEM PARAMETER - Raw Load current sensor ADC voltages
        csiConverted = 0.0000,            // SYSTEM PARAMETER - Actual Input current sensor ADC voltage
        csbConverted = 0.0000,            // SYSTEM PARAMETER - Actual Battery current sensor ADC voltage
        cslConverted = 0.0000,            // SYSTEM PARAMETER - Actual Load current sensor ADC voltage
        TS = 0.0000,                      // SYSTEM PARAMETER - Raw temperature sensor ADC value
        TSlog = 0.0000,                   // SYSTEM PARAMETER - Part of NTC thermistor thermal sensing code
        sampleStoreTS = 0,                // SYSTEM PARAMETER - TS AVG nth Sample
        inVoltageDivRatio = 40.2156,      //  CALIB PARAMETER - Input voltage divider sensor ratio (change this value to calibrate voltage sensor)
        batteryVoltageDivRatio = 24.4300, //  CALIB PARAMETER - Output voltage divider sensor ratio (change this value to calibrate voltage sensor)
        loadVoltageDivRatio = 24.3500,    //  CALIB PARAMETER - Output voltage divider sensor ratio (change this value to calibrate voltage sensor)
        currentSens = 0.0000,             //  CALIB PARAMETER - Current Sensor Sensitivity (V/A)
        currentSensVInput = 0.0600,       //  CALIB PARAMETER - Current Sensor Sensitivity (mV/A)
        currentSensVBatt = 0.0750,        //  CALIB PARAMETER - Current Sensor Sensitivity (mV/A)
        currentSensVLoad = 0.0759,        //  CALIB PARAMETER - Current Sensor Sensitivity (mV/A)
        currentSensV = 0.0600,            //  CALIB PARAMETER - Current Sensor Sensitivity (mV/A)
        WhInput = 0.0000,                 // SYSTEM PARAMETER - Stores the accumulated energy harvested (Watt-Hours)
        kWhInput = 0.0000,                // SYSTEM PARAMETER - Stores the accumulated energy harvested (Kiliowatt-Hours)
        MWhInput = 0.0000,                // SYSTEM PARAMETER - Stores the accumulated energy harvested (Megawatt-Hours)
        WhOutput = 0.0000,                // SYSTEM PARAMETER - Stores the accumulated energy Usage (Watt-Hours)
        kWhOutput = 0.0000,               // SYSTEM PARAMETER - Stores the accumulated energy Usage (Kiliowatt-Hours)
        MWhOutput = 0.0000,               // SYSTEM PARAMETER - Stores the accumulated energy Usage (Megawatt-Hours)
        outputDeviation = 0.0000;         // SYSTEM PARAMETER - Output Voltage Deviation (%)

    int
        millisRoutineInterval = 250, //  USER PARAMETER - Time Interval Refresh Rate For Routine Functions (ms)
        temperature = 0,             // SYSTEM PARAMETER -
        batteryPercent = 0,          // SYSTEM PARAMETER -
        inputSource = USB_SOURCE;    // SYSTEM PARAMETER - 0 = MPPT has no power source, 1 = MPPT is using solar as source, 2 = MPPTis using battery as power source
};

#endif