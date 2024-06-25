#include "sensor_reading_task.h"
#include "task_aruments.h"

void sensorReadingTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    SensorReading *sensor = argument->sensor;
    SemaphoreHandle_t readSensorMutex = argument->readSensorMutex;

    while (true)
    {
        if (xSemaphoreTake(readSensorMutex, portMAX_DELAY))
        {
            sensor->readSensor();
            xSemaphoreGive(readSensorMutex);
        }
        vTaskDelay(1 / portTICK_RATE_MS);
    }
}