#include "system_process_task.h"
#include "task_aruments.h"

void systemProcessTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    SensorReading *sensor = argument->sensor;
    SystemProcess *systemProcess = argument->systemProcess;

    while (true)
    {
        systemProcess->fanSystem();
        systemProcess->timeCounting();
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}