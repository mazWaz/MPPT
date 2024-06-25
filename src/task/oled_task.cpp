#include "oled_task.h"
#include "task_aruments.h"

void oledTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    OledDisplay *oledDisplay = argument->oledDisplay;
    while (true)
    {
        
        oledDisplay->displayMenu();
        vTaskDelay(33 / portTICK_PERIOD_MS);
    }
}