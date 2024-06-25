#include "serial_telemetry_task.h"
#include "task_aruments.h"

void serialTelemetryTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    SensorReading *sensor = argument->sensor;
    DeviceProtection *deviceProtection = argument->deviceProtection;
    SystemProcess *systemProcess = argument->systemProcess;
    ChargingAlgorithm *chargingAlgorithm = argument->chargingAlgorithm;

    while (true)
    {
        if (SERIAL_TELE_MODE == DISABLE)
        {
        }
        else if (SERIAL_TELE_MODE == DISPLAY_ALL)
        {
            Serial.print(" ERR:");
            Serial.print(deviceProtection->getERR());
            Serial.print(" FLV:");
            Serial.print(deviceProtection->getFLV());
            Serial.print(" BNC:");
            Serial.print(deviceProtection->getBNC());
            Serial.print(" IUV:");
            Serial.print(deviceProtection->getIUV());
            Serial.print(" IOC:");
            Serial.print(deviceProtection->getIOC());
            Serial.print(" OOV:");
            Serial.print(deviceProtection->getOOV());
            Serial.print(" OOC:");
            Serial.print(deviceProtection->getOOC());
            Serial.print(" OTE:");
            Serial.print(deviceProtection->getOTE());
            Serial.print(" REC:");
            Serial.print(deviceProtection->getREC());

            Serial.print(" MPPTA:");
            Serial.print(systemProcess->getMPPTMode());
            Serial.print(" CM:");
            Serial.print(systemProcess->getOutputMode());

            Serial.print(" ");
            Serial.print(" BYP:");
            Serial.print(deviceProtection->getBypassEnable());
            Serial.print(" EN:");
            Serial.print(chargingAlgorithm->getBuckState());
            Serial.print(" FAN:");
            Serial.print(systemProcess->getMPPTMode());
            // Serial.print(" WiFi:");
            // Serial.print(WIFI);
            Serial.print(" ");
            Serial.print(" PI:");
            Serial.print(sensor->getPowerInput(), 0);
            Serial.print(" PWM_MAX:");
            Serial.print(chargingAlgorithm->getPwmMax(), 0);
            Serial.print(" PWM_MAX_LIIMMITED:");
            Serial.print(chargingAlgorithm->getPwmMaxLimited(), 0);
            Serial.print(" PWM:");
            Serial.print(chargingAlgorithm->getPwm());
            Serial.print(" PPWM:");
            Serial.print(chargingAlgorithm->getPpwm());
            Serial.print(" VI:");
            Serial.print(sensor->getVoltageInput(), 1);
            Serial.print(" VB:");
            Serial.print(sensor->getVoltageBattery(), 1);
            Serial.print(" VL:");
            Serial.print(sensor->getVoltageLoad(), 1);
            Serial.print(" CI:");
            Serial.print(sensor->getCurrentInput(), 2);
            Serial.print(" CB:");
            Serial.print(sensor->getCurrentBattery(), 2);
            Serial.print(" CL:");
            Serial.print(sensor->getPowerLoad(), 2);
            Serial.print(" Wh:");
            Serial.print(sensor->getWhInput(), 2);
            Serial.print(" Temp:");
            Serial.print(sensor->getTemperature(), 1);
            Serial.print(" ");
            Serial.print(" CSMPV:");
            Serial.print(sensor->getCurrentMidPoint(), 3);
            Serial.print(" CSV:");
            Serial.print(sensor->getCsiConverted(), 3);
            Serial.print(" VO%Dev:");
            Serial.print(sensor->getOutputDeviation(), 1);
            Serial.print(" SOC:");
            Serial.print(sensor->getBattPercent());
            Serial.print("%");
            Serial.print("T:");
            Serial.print(systemProcess->getSecondsElapsed());
            Serial.print(" LoopT Charing:");
            Serial.print(chargingAlgorithm->getChargingLoopTimme(), 3);
            Serial.print("ms");
            Serial.print(" LoopT Sensor:");
            Serial.print(sensor->getSensorLoopTimme(), 3);
            Serial.print("ms");
            Serial.println("");
        }
        else if (SERIAL_TELE_MODE == ESSENTIAL_DATA)
        {
            Serial.print(" PI:");
            Serial.print(sensor->getPowerInput(), 0);
            Serial.print(" PWM:");
            Serial.print(chargingAlgorithm->getPwm());
            Serial.print(" PPWM:");
            Serial.print(chargingAlgorithm->getPpwm());
            Serial.print(" VI:");
            Serial.print(sensor->getVoltageInput(), 1);
            Serial.print(" VB:");
            Serial.print(sensor->getVoltageBattery(), 1);
            Serial.print(" VL:");
            Serial.print(sensor->getVoltageLoad(), 1);
            Serial.print(" CI:");
            Serial.print(sensor->getCurrentInput(), 2);
            Serial.print(" CB:");
            Serial.print(sensor->getCurrentBattery(), 2);
            Serial.print(" CL:");
            Serial.print(sensor->getPowerLoad(), 2);
            Serial.print(" Wh:");
            Serial.print(sensor->getWhInput(), 2);
            Serial.print(" Temp:");
            Serial.print(sensor->getTemperature(), 1);
            Serial.print(" EN:");
            Serial.print(chargingAlgorithm->getBuckState());
            Serial.print(" FAN:");
            Serial.print(systemProcess->getMPPTMode());
            Serial.print(" SOC:");
            Serial.print(sensor->getBattPercent());
            Serial.print("%");
            Serial.print("T:");
            Serial.print(systemProcess->getSecondsElapsed());
            Serial.print(" LoopT:");
            Serial.print(chargingAlgorithm->getChargingLoopTimme(), 3);
            Serial.print("ms");
            Serial.println("");
        }
        else if (SERIAL_TELE_MODE == DISPLAY_NUMBER)
        {
            Serial.print(" ");
            Serial.print(sensor->getPowerInput(), 0);
            Serial.print(" ");
            Serial.print(sensor->getVoltageInput(), 1);
            Serial.print(" ");
            Serial.print(sensor->getVoltageBattery(), 1);
            Serial.print(" ");
            Serial.print(sensor->getVoltageLoad(), 1);
            Serial.print(" ");
            Serial.print(sensor->getCurrentInput(), 2);
            Serial.print(" ");
            Serial.print(sensor->getCurrentBattery(), 2);
            Serial.print(" ");
            Serial.print(sensor->getPowerLoad(), 2);
            Serial.print(" ");
            Serial.print(sensor->getWhInput(), 2);
            Serial.print(" ");
            Serial.print(sensor->getTemperature(), 1);
            Serial.print(" ");
            Serial.print(chargingAlgorithm->getBuckState());
            Serial.print(" ");
            Serial.print(systemProcess->getMPPTMode());
            Serial.print(" ");
            Serial.print(sensor->getBattPercent());
            Serial.print(" ");
            Serial.print(systemProcess->getSecondsElapsed());
            Serial.print(" ");
            Serial.print(chargingAlgorithm->getChargingLoopTimme(), 3);
            Serial.print(" ");
        }

        vTaskDelay(800 / portTICK_RATE_MS);
    }
}