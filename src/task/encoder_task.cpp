#include "encoder_task.h"
#include "task_aruments.h"

void encoderTask(void *pvParameter)
{
    volatile TaskArg *argument = (TaskArg *)pvParameter;
    Encoder *encoder = argument->encoder;
    encoder->setup();

    while (true)
    {
        encoder->rotaryRead();

        encoder->pushButtonStart();
        // byte encoderSinggle = encoder->singglePush();
        // if (encoderSinggle)
        // {
        //     // Serial.println(encoder->getPosition());
        // }
        byte encoderDouble = encoder->dublePush();
        if (encoderDouble)
        {
            encoder->resetPosition();
        }

        byte encoderTriple = encoder->triplePush();
        if (encoderTriple)
        {
            encoder->setPosition(10);
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}