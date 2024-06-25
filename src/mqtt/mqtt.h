#ifndef MQTT_H
#define MQTT_H

#include "config.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "./sensor/sensor_reading.h"
#include "./system_process/system_process.h"
#include "./wifi_man/wifi_man.h"

class SensorReading;
class SystemProcess;
class WifiMan;

class Mqtt
{
public:
    Mqtt() : client(espClient){};

    void setup(SystemProcess *system_process, WifiMan *wifi_man, SensorReading *sensors);
    void mqtt();

private:
    WiFiClient espClient;
    PubSubClient client;

    SensorReading *sensor;
    SystemProcess *systemProcess;
    WifiMan *wifiMan;

    void mqttCallback(char *topic, uint8_t *message, unsigned int length);
    void mqttReconect();
};

#endif
