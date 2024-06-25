#include "device_protection.h"

DeviceProtection::DeviceProtection(uint8_t backflow_mosfet)
{
    backflowMosfet = backflow_mosfet;
};

void DeviceProtection::setup(SystemProcess *system_process, SensorReading *sensors)
{
    systemProcess = system_process;
    sensor = sensors;
    pinMode(backflowMosfet, OUTPUT);
}

void DeviceProtection::backflowControl() // PV BACKFLOW CONTROL (INPUT MOSFET)
{
    if (!systemProcess->getOutputMode()) // PSU MODE: Force backflow MOSFET on
    {
        bypassEnable = true; // CHARGER MODE: Force backflow MOSFET on
    }
    else
    {
        if (sensor->getVoltageInput() > sensor->getVoltageBattery() + voltageDropout)
        {
            bypassEnable = true; // CHARGER MODE: Normal Condition - Turn on Backflow MOSFET (on by default when not in MPPT charger mode)
        }
        else
        {
            bypassEnable = false; // CHARGER MODE: Input Undervoltage - Turn off bypass MOSFET and prevent PV Backflow (SS)
        }
    }
    digitalWrite(backflowMosfet, bypassEnable); // Signal backflow MOSFET GPIO pin
}

void DeviceProtection::deviceProtection()
{
    currentRoutineMillis = millis();
    if (currentErrorMillis - prevErrorMillis >= errorTimeLimit) // Run routine every millisErrorInterval (ms)
    {
        prevErrorMillis = currentErrorMillis; // Store previous time
        if (errorCount < errorCountLimit)     // Reset error count if it is below the limit before x milliseconds
        {
            errorCount = 0;
        }
    }
    // FAULT DETECTION
    ERR = 0;           // Reset local error counter
    backflowControl(); // Run backflow current protection protocol

    if (sensor->getTemperature() > systemProcess->getTemperatureMax()) // OTE - OVERTEMPERATURE: System overheat detected
    {
        OTE = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        OTE = 0;
    }

    if (sensor->getCurrentInput() > currentInAbsolute) // IOC - INPUT  OVERCURRENT: Input current has reached absolute limit
    {
        IOC = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        IOC = 0;
    }

    if (sensor->getCurrentBattery() > currentOutAbsolute) // OOC - OUTPUT OVERCURRENT: Output current has reached absolute limit
    {
        OOC = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        OOC = 0;
    }

    if (sensor->getVoltageBattery() > systemProcess->getVoltageBatteryMax() + voltageBatteryThresh) // OOV - OUTPUT OVERVOLTAGE: Output voltage has reached absolute limit
    {
        OOV = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        OOV = 0;
    }

    if (sensor->getVoltageInput() < vInSystemMin && sensor->getVoltageBattery() < vInSystemMin) // FLV - Fatally low system voltage (unable to resume operation)
    {
        FLV = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        FLV = 0;
    }

    if (systemProcess->getOutputMode() == 0) // PSU MODE specific protection protocol
    {
        REC = 0;                                                                                // Clear recovery and battery not connected boolean identifiers
        BNC = 0;                                                                                // Clear recovery and battery not connected boolean identifiers
        if (sensor->getVoltageInput() < systemProcess->getVoltageBatteryMax() + voltageDropout) // IUV - INPUT UNDERVOLTAGE: Input voltage is below battery voltage (for psu mode only)
        {
            IUV = 1;
            ERR++;
            errorCount++;
        }
        else
        {
            IUV = 0;
        }
    }
    else
    {                                                   // Charger MODE specific protection protocol
        backflowControl();                              // Enable backflow current detection & control
        if (sensor->getVoltageBattery() < vInSystemMin) // BNC - BATTERY NOT CONNECTED (for charger mode only, does not treat BNC as error when not under MPPT mode)
        {
            BNC = 1;
            ERR++;
        }
        else
        {
            BNC = 0;
        }
        if (sensor->getVoltageInput() < systemProcess->getVoltageBatteryMax() + voltageDropout) // IUV - INPUT UNDERVOLTAGE: Input voltage is below max battery charging voltage (for charger mode only)
        {
            IUV = 1;
            ERR++;
            REC = 1;
        }
        else
        {
            IUV = 0;
        }
    }
}