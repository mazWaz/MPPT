#include "encoder.h"

Encoder::Encoder(uint8_t encoder_clk, uint8_t encoder_dt, uint8_t encoder_push)
{

    encoderCLK = encoder_clk;
    encoderDT = encoder_dt;
    encoderPush = encoder_push;
};

void Encoder::setup()
{
    pinMode(encoderCLK, INPUT);
    pinMode(encoderDT, INPUT);
    pinMode(encoderPush, INPUT_PULLUP);

    previousCLK = digitalRead(encoderCLK);
    previousDATA = digitalRead(encoderDT);
}

void Encoder::rotaryRead()
{

    /* if ((previousCLK == 0) && (previousDATA == 1))
     {
         if ((digitalRead(encoderCLK) == 1) && (digitalRead(encoderDT) == 0))
         {
             Serial.println("CW1");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 1;
         }
         if ((digitalRead(encoderCLK) == 1) && (digitalRead(encoderDT) == 1))
         {
             Serial.println("CCW1");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 2;
         }
     }

     if ((previousCLK == 1) && (previousDATA == 0))
     {
         if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 1))
         {
             Serial.println("CW2");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 1;
         }
         if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 0))
         {
             Serial.println("CCW2");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 2;
         }
     }

     if ((previousCLK == 1) && (previousDATA == 1))
     {
         if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 1))
         {
             Serial.println("CW3");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 1;
         }
         if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 0))
         {
             Serial.println("CCW3");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 2;
         }
     }
     if ((previousCLK == 0) && (previousDATA == 0))
     {
         if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 1))
         {
             Serial.println("CW4");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 1;
         }
         if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 0))
         {
             Serial.println("CCW4");
             previousCLK = digitalRead(encoderCLK);
             previousDATA = digitalRead(encoderDT);
             return 2;
         }
     }

     */

    if ((previousCLK == 1) && (previousDATA == 0))
    {
        if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 1))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
            if (!hold)
            {
                value = 0;
                position--;
            }
            else
            {
                if (value > 0)
                {
                    value--;
                }
            }
        }
        if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 0))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
        }
    }

    if ((previousCLK == 1) && (previousDATA == 1))
    {
        if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 1))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
        }
        if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 0))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
            if (!hold)
            {
                value = 0;
                position++;
            }
            else
            {
                if (value >= 0)
                {
                    value++;
                }
            }
        }
    }
    if ((previousCLK == 0) && (previousDATA == 0))
    {
        if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 1))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
        }
        if ((digitalRead(encoderCLK) == 0) && (digitalRead(encoderDT) == 0))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
        }
    }
    if ((previousCLK == 0) && (previousDATA == 1))
    {
        if ((digitalRead(encoderCLK) == 1) && (digitalRead(encoderDT) == 0))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
        }
        if ((digitalRead(encoderCLK) == 1) && (digitalRead(encoderDT) == 1))
        {

            previousCLK = digitalRead(encoderCLK);
            previousDATA = digitalRead(encoderDT);
        }
    }
};

void Encoder::pushButtonStart()
{
    if (!digitalRead(encoderPush))
    {
        if (millis() - lastButtonPress > 250)
        {
            tapCounter += 1;
            lastButtonPress = millis();
        }
    }

    if (millis() - lastButtonPress > 1000)
    {
        tapCounter = 0;
    }
}

byte Encoder::singglePush()
{
    if (millis() - lastButtonPress > 500 && tapCounter == 1)
    {
        tapCounter = 0;
        return 1;
    }

    return 0;
}

byte Encoder::dublePush()
{
    if (millis() - lastButtonPress > 500 && tapCounter == 2)
    {
        tapCounter = 0;
        return 1;
    }
    return 0;
}

byte Encoder::triplePush()
{
    if (millis() - lastButtonPress > 500 && tapCounter == 3)
    {
        tapCounter = 0;
        return 1;
    }
    return 0;
}