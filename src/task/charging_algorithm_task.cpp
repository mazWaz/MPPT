#include "charging_algorithm_task.h"
#include "task_aruments.h"

void charginAlgorithmTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    ChargingAlgorithm *chargingAlgorithm = argument->chargingAlgorithm;
    SensorReading *sensor = argument->sensor;
    DeviceProtection *deviceProtection = argument->deviceProtection;
    SystemProcess *systemProcess = argument->systemProcess;
    SemaphoreHandle_t readSensorMutex = argument->readSensorMutex;

    while (true)
    {
        if (xSemaphoreTake(readSensorMutex, (1 * portTICK_PERIOD_MS)))
        {
            chargingAlgorithm->chargingAlgorithm();
            xSemaphoreGive(readSensorMutex);
        }

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}