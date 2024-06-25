#include "mqtt_task.h"
#include "task_aruments.h"

void mqttTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    Mqtt *mqtt = argument->mqtt;

    while (true)
    {
        mqtt->mqtt();
        vTaskDelay(MQTT_SEND_DELAY / portTICK_PERIOD_MS);
    }
}