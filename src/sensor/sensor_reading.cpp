#include "sensor_reading.h"

SensorReading::SensorReading(uint8_t address_one, uint8_t address_two, uint8_t temp_sensor)
{
    addressOne = address_one;
    addressTwo = address_two;
    tempSensor = temp_sensor;
}

void SensorReading::setup(SystemProcess *system_process, DeviceProtection *device_protection, ChargingAlgorithm *charging_algorithm)
{
    systemProcess = system_process;
    deviceProtection = device_protection;
    chargingAlgorithm = charging_algorithm;

    switch (ADC_GAIN_SELECT)
    {
    case G_TWOTHIRDS:
        adsOne.setGain(GAIN_TWOTHIRDS);
        adsTwo.setGain(GAIN_TWOTHIRDS);
        break;
    case G_ONE:
        adsOne.setGain(GAIN_ONE);
        adsTwo.setGain(GAIN_ONE);
        break;
    case G_TWO:
        adsOne.setGain(GAIN_TWO);
        adsTwo.setGain(GAIN_TWO);
        break;

    default:
        break;
    }
    adsOne.begin(addressOne);
    adsTwo.begin(addressTwo);
    Wire.setClock(3400000);
}

void SensorReading::readSensor()
{
    loopTimeStart = micros();                                   // Record Start Time
    sensorLoopTimme = (loopTimeStart - loopTimeEnd) / 1000.000; // Compute Loop Cycle Speed (mS)
    loopTimeEnd = micros();                                     // Record End Time

    currentSensorCalibration();
    readVoltageAmpere();
    sourceDetection();
    readTemperature();
    powerComputation();
    batteryPercentage();
    sensorDataComputation();
}

void SensorReading::readVoltageAmpere()
{
    vsi = 0.0000; // Clear Previus Input Voltage
    vsb = 0.0000; // Clear Previus Battery Voltage
    vsl = 0.0000; // Clear Previus Load Voltage

    csi = 0.0000; // Clear Previous input Current
    csb = 0.0000; // Clear Previous Battery Current
    csl = 0.0000; // Clear Previous load Current
    for (int i = 0; i < AVG_COUNT_VOLTAGE; i++)
    {
        vsi = vsi + adsOne.computeVolts(adsOne.readADC_SingleEnded(VOLTAGE_ANALOG_INPUT));
        vsb = vsb + adsOne.computeVolts(adsOne.readADC_SingleEnded(VOLTAGE_ANALOG_BATTERY));
        vsl = vsl + adsTwo.computeVolts(adsTwo.readADC_SingleEnded(VOLTAGE_ANALOG_LOAD));
    }

    for (int i = 0; i < AVG_COUNT_CURRENT; i++)
    {
        csi = csi + adsOne.computeVolts(adsOne.readADC_SingleEnded(AMPERE_ANALOG_INPUT));
        csb = csb + adsOne.computeVolts(adsOne.readADC_SingleEnded(AMPERE_ANALOG_BATTERY));
        csl = csl + adsTwo.computeVolts(adsTwo.readADC_SingleEnded(AMPERE_ANALOG_LOAD));
    }
    voltageInput = (vsi / AVG_COUNT_VOLTAGE) * inVoltageDivRatio;
    voltageBattery = (vsb / AVG_COUNT_VOLTAGE) * batteryVoltageDivRatio;
    voltageLoad = (vsl / AVG_COUNT_VOLTAGE) * loadVoltageDivRatio;

    csiConverted = (csi / AVG_COUNT_CURRENT) * 1.3300;
    csbConverted = (csb / AVG_COUNT_CURRENT) * 1.3300;
    cslConverted = (csl / AVG_COUNT_CURRENT) * 1.3300;

    currentInput = ((csiConverted - currentMidPointiInput) * -1) / currentSensVInput;
    currentBattery = ((csbConverted - currentMidPointBatt) * -1) / currentSensVBatt;
    currentLoad = ((cslConverted - currentMidPointLoad) * -1) / currentSensVLoad;
    currentBattery = currentBattery - currentLoad;

    if (currentInput < 0)
    {
        currentInput = 0.0000;
    }

    if (currentBattery < 0)
    {
        currentBattery = 0.0000;
    }

    if (currentLoad < 0)
    {
        currentLoad = 0.0000;
    }
}

void SensorReading::sourceDetection()
{
    if (voltageInput <= 3 && voltageBattery <= 3)
    {
        inputSource = USB_SOURCE;
    }
    else if (voltageInput > voltageBattery)
    {
        inputSource = INPUT_SOURCE;
    }
    else if (voltageInput < voltageBattery)
    {
        inputSource = BATTERY_SOURCE;
    }
}

void SensorReading::readTemperature()
{
    if (sampleStoreTS <= AVG_COUNT_TEMPERATURE)
    { // TEMPERATURE SENSOR - Lite Averaging
        TS = TS + analogRead(tempSensor);
        sampleStoreTS++;
    }
    else
    {
        TS = TS / sampleStoreTS;
        TSlog = log(NTC_RESISTANSE * (4095.00 / TS - 1.00));
        temperature = (1.0 / (1.009249522e-03 + 2.378405444e-04 * TSlog + 2.019202697e-07 * TSlog * TSlog * TSlog)) - 273.15;
        sampleStoreTS = 0;
        TS = 0;
    }
}

void SensorReading::currentSensorCalibration()
{
    if (!chargingAlgorithm->getBuckState() && deviceProtection->getFLV() == 0 && deviceProtection->getOOV() == 0)
    {
        currentMidPointiInput = ((csi / AVG_COUNT_CURRENT) * 1.3300) - 0.003;
        currentMidPointBatt = ((csb / AVG_COUNT_CURRENT) * 1.3300) - 0.003;
        currentMidPointLoad = ((csl / AVG_COUNT_CURRENT) * 1.3300) - 0.003;
    }
}

void SensorReading::powerComputation()
{
    powerInput = voltageInput * currentInput;
    powerLoad = voltageLoad * currentLoad;
    powerBattery = (voltageBattery * currentBattery) - powerLoad;
    outputDeviation = (voltageBattery * systemProcess->getVoltageBatteryMax()) * 100;
}

void SensorReading::batteryPercentage()
{
    batteryPercent = ((voltageBattery - systemProcess->getVoltageBatteryMin()) / (systemProcess->getVoltageBatteryMax() - systemProcess->getVoltageBatteryMin())) * 101;
    batteryPercent = constrain(batteryPercent, 0, 100);
}

void SensorReading::sensorDataComputation()
{
    currentRoutineMillis = millis();
    if (currentRoutineMillis - prevRoutineMillis >= millisRoutineInterval)
    {                                                                                       // Run routine every millisRoutineInterval (ms)
        prevRoutineMillis = currentRoutineMillis;                                           // Store previous time
        WhInput = WhInput + (powerInput / (3600.000 * (1000.000 / millisRoutineInterval))); // Accumulate and compute energy harvested (3600s*(1000/interval))
        kWhInput = WhInput / 1000.000;
        MWhInput = WhInput / 1000000.000;

        WhOutput = WhOutput + (powerLoad / (3600.000 * (1000.000 / millisRoutineInterval))); // Accumulate and compute energy harvested (3600s*(1000/interval))
        kWhOutput = WhOutput / 1000.000;
        MWhOutput = WhOutput / 1000000.000;
    }
}