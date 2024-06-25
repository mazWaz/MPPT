#include "device_protection_task.h"
#include "task_aruments.h"

void deviceProtectionTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    SensorReading *sensor = argument->sensor;
    DeviceProtection *deviceProtection = argument->deviceProtection;
    SystemProcess *systemProcess = argument->systemProcess;

    while (true)
    {
        deviceProtection->deviceProtection();

        vTaskDelay(100 / portTICK_RATE_MS);
    }
}