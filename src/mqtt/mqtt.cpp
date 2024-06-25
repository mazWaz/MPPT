#include "mqtt.h"

void Mqtt::setup(SystemProcess *system_process, WifiMan *wifi_man, SensorReading *sensors)
{
    sensor = sensors;
    systemProcess = system_process;
    wifiMan = wifi_man;

    client.setServer(systemProcess->getMqttServer().c_str(), systemProcess->getMqttPort());
    client.connect(
        systemProcess->getMqttEndPoint().c_str(),
        systemProcess->getMqttUser().c_str(),
        systemProcess->getMqttPassword().c_str());

    client.subscribe(SUBSCRIBE_RELAY);
    client.setCallback(
        std::bind(
            &Mqtt::mqttCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
}

void Mqtt::mqtt()
{
    if (client.connected())
    {
        wifiMan->setWebMqttConnection(true);
    }
    else
    {
        wifiMan->setWebMqttConnection(false);
        mqttReconect();
    }
    client.loop();
    // Sensor
    const size_t capacitySensor = JSON_OBJECT_SIZE(19); // Set the capacity as per your JSON structure
    StaticJsonDocument<capacitySensor> sensorDoc;

    // Add data to the JSON object
    switch (sensor->getInputSource())
    {
    case 0:
        sensorDoc["inputSource"] = "USB";
        sensorDoc["isPowerShutdown"] = true;
        break;
    case 1:
        sensorDoc["inputSource"] = "PSU";
        sensorDoc["isPowerShutdown"] = false;
        break;
    case 2:
        sensorDoc["inputSource"] = "Battery";
        sensorDoc["isPowerShutdown"] = true;
        break;

    default:
        break;
    }
    sensorDoc["deviceTemperature"] = sensor->getTemperature();
    sensorDoc["voltageInput"] = sensor->getVoltageInput();
    sensorDoc["currentInput"] = sensor->getCurrentInput();
    sensorDoc["powerInput"] = sensor->getPowerInput();
    sensorDoc["whInput"] = sensor->getWhInput();
    sensorDoc["kwhInput"] = sensor->getKwhInput();
    sensorDoc["mwhInput"] = sensor->getMWhInput();

    sensorDoc["voltageBattery"] = sensor->getVoltageBattery();
    sensorDoc["currentBattery"] = sensor->getCurrentBattery();
    sensorDoc["powerBattery"] = sensor->getPowerBattery();
    sensorDoc["percentageBattery"] = sensor->getBattPercent();

    sensorDoc["voltageLoad"] = sensor->getVoltageLoad();
    sensorDoc["currentLoad"] = sensor->getCurrentLoad();
    sensorDoc["powerLoad"] = sensor->getPowerLoad();
    sensorDoc["whLoad"] = sensor->getWhOutput();
    sensorDoc["kwhLoad"] = sensor->getKwhOutput();
    sensorDoc["mwhLoad"] = sensor->getMWhOutput();

    // Serialize the JSON object into a char buffer
    char sensorBuffer[512];
    size_t sensorLen = serializeJson(sensorDoc, sensorBuffer, sizeof(sensorBuffer));
    client.publish(PUBLISH_SENSOR_TOPIC, sensorBuffer, sensorLen);
    Serial.println(sensorBuffer);

    // CONFIG

    const size_t capacityConfig = JSON_OBJECT_SIZE(19); // Set the capacity as per your JSON structure
    StaticJsonDocument<capacityConfig> configDoc;
    configDoc["upTime"] = String(millis() / 1000 / 60) + "mins" + String((String)((millis() / 1000) % 60)) + "secs";
    configDoc["deviceName"] = CLIENT_NAME;
    configDoc["ssid"] = wifiMan->getSSID();
    configDoc["ipAddress"] = WiFi.localIP();
    configDoc["macAddress"] = WiFi.macAddress().c_str();

    char configBuffer[256];
    size_t configLen = serializeJson(configDoc, configBuffer, sizeof(configBuffer));
    client.publish(PUBLISH_CONFIG_TOPIC, configBuffer, configLen);
    Serial.println(configBuffer);
}

void Mqtt::mqttReconect()
{
    while (!client.connected())
    {

        client.setServer(systemProcess->getMqttServer().c_str(), systemProcess->getMqttPort());
        if (client.connect(
                systemProcess->getMqttEndPoint().c_str(),
                systemProcess->getMqttUser().c_str(),
                systemProcess->getMqttPassword().c_str()))
        {

            client.subscribe(SUBSCRIBE_RELAY);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void Mqtt::mqttCallback(char *topic, uint8_t *message, unsigned int length)
{
    String strTopic(topic);
    String messageTemp;
    Serial.print("Message arrived on topic: ");
    Serial.print(strTopic);
    Serial.print(". Message: ");
    for (unsigned int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    if (strTopic == String(SUBSCRIBE_RELAY))
    {
        if (messageTemp == "1")
        {

            systemProcess->setRelay(true);
        }
        else if (messageTemp == "0")
        {
            systemProcess->setRelay(false);
        }
    }
    Serial.println();
}