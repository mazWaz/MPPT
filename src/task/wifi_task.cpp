#include "wifi_task.h"
#include "task_aruments.h"

void wifiTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    WifiMan *wifiMan = argument->wifiMan;

    while (true)
    {
        wifiMan->wifiMan();
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}