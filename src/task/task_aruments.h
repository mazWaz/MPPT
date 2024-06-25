#ifndef TASKS_ARGUMENTS_H
#define TASKS_ARGUMENTS_H

#include "./sensor/sensor_reading.h"
#include "./system_process/system_process.h"
#include "./device_protection/device_protection.h"
#include "./charging_algorithm/charging_algorithm.h"
#include "./encoder/encoder.h"
#include "./wifi_man/wifi_man.h"
#include "./oled_display/oled.h"
#include "./mqtt/mqtt.h"

typedef struct
{
    SensorReading *sensor;
    SystemProcess *systemProcess;
    DeviceProtection *deviceProtection;
    ChargingAlgorithm *chargingAlgorithm;
    Encoder *encoder;
    OledDisplay *oledDisplay;
    WifiMan *wifiMan;
    Mqtt *mqtt;
    SemaphoreHandle_t readSensorMutex;
} TaskArg;

#endif