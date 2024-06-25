#ifndef ENCODER_H
#define ENCODER_H
#include "config.h"

class Encoder
{
public:
    Encoder(uint8_t encoder_clk, uint8_t encoder_dt, uint8_t encoder_push);
    void setup();
    void pushButtonStart();
    void rotaryRead();

    int getPosition()
    {
        return position;
    };

    int getValue()
    {
        return value;
    }

    void setValue(int values)
    {
        value = values;
    }

    void setPosition(int position_value)
    {
        position = position_value;
    };
    void resetPosition()
    {
        position = 0;
    };

    void holdPosition()
    {
        hold = true;
    }

    void releasePosition()
    {
        hold = false;
    }

    byte singglePush();
    byte dublePush();
    byte triplePush();

private:
    bool hold = false;
    int
        position = 0,
        value = 0,
        timediff,
        tapCounter = 0;
    uint8_t
        encoderCLK,
        encoderDT,
        encoderPush;
    byte
        previousCLK,
        previousDATA;
    unsigned long
        lastButtonPress = 0,
        debunceButton = 50;
};

#endif