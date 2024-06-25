#ifndef SYSTEM_PROCESS_H
#define SYSTEM_PROCESS_H

#include "config.h"
// #include <EEPROM.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "./sensor/sensor_reading.h"
#include "./wifi_man/wifi_man.h"

class SensorReading;
class WifiMan;

class SystemProcess
{
public:
    SystemProcess(uint8_t fan);
    void setup(SensorReading *_sensor, WifiMan *_wifiMan, uint8_t _relay);
    void resetVariables();
    void saveAutoloadSettings();
    void timeCounting();
    void factoryReset();

    void fanSystem();

    void saveMPPTMode(bool mppt_value);
    void saveOutputMode(bool output_mode);
    void saveVoltageBatteryMax(float voltage_batt_max);
    void saveVoltageBatteryMin(float voltage_batt_min);
    void saveCurrentCharging(float current_charging);
    void saveEnableFan(bool enable_fan);
    void saveTemperatureFan(int temperature_fan);
    void saveTemperatureMax(int temperature_max);
    void saveEnableWifi(bool enable_wifi);
    void saveBacklightSleepMode(int backlight_sleep_mode);
    void saveParamMqtt(String mqtt_end_point, String mqtt_server, String mqtt_port, String mqtt_user, String mqtt_password);

    void setTimeOn(int time_on)
    {
        timeOn = time_on;
    };
    void setDaysRunning(float days_running) { daysRunning = days_running; };
    void setMPPTMode(bool mppt_mode) { MPPTMode = mppt_mode; };
    void setOutputMode(bool output_mode) { outputMode = output_mode; };
    void setVoltageBatteryMax(float voltage_batt_max) { voltageBatteryMax = voltage_batt_max; };
    void setvoltageBatteryMin(float voltage_batt_min) { voltageBatteryMin = voltage_batt_min; };
    void setCurrentCharging(float current_charging) { currentCharging = current_charging; };
    void setFanStatus(bool enable_fan) { enableFan = enable_fan; };
    void setTemperatureFan(int temperature_fan) { temperatureFan = temperature_fan; };
    void setTemperatureMax(int temperature_max) { temperatureMax = temperature_max; };
    void setRelay(bool relayState);

    int getTimeOn()
    {
        return timeOn;
    };
    int getTemperatureMax() { return temperatureMax; };
    float getVoltageBatteryMax() { return voltageBatteryMax; }
    float getVoltageBatteryMin() { return voltageBatteryMin; }
    float getCurrentCharging() { return currentCharging; };
    float getDayRunning() { return daysRunning; };
    bool getOutputMode() { return outputMode; };
    bool getMPPTMode() { return MPPTMode; };
    bool getFanStatus() { return enableFan; };
    int getTemperatureFan() { return temperatureFan; };
    unsigned long getSecondsElapsed() { return secondsElapsed; };
    String getMqttEndPoint() { return mqttEndpoint; };
    String getMqttServer() { return mqttServer; };
    int getMqttPort() { return mqttPort; };
    String getMqttUser() { return mqttUser; }
    String getMqttPassword() { return mqttPassword; };

private:
    SensorReading *sensor;
    WifiMan *wifiMan;

    uint8_t fan;

    struct Config
    {
        bool mpptMode;
        bool outputMode;
        float voltageBatteryMax;
        float voltageBatteryMin;
        float currentCharging;
        bool enableFan;
        int temperatureFan;
        int temperatureMax;
        bool saveEnableWifi;
        int backlightSleepMode;
        bool autoLoadSetting;
        char mqttEndpoint[64];
        char mqttServer[64];
        int mqttPort;
        char mqttUser[64];
        char mqttPassword[64];
    } config;

    int
        millisRoutineInterval = 250, //  USER PARAMETER - Time Interval Refresh Rate For Routine Functions (ms)
        timeOn = 0,                  // SYSTEM PARAMETER -
        conv1 = 0,                   // SYSTEM PARAMETER -
        conv2 = 0,                   // SYSTEM PARAMETER -
        temperatureFan = 50,         //  USER PARAMETER - Temperature threshold for fan to turn on
        temperatureMax = 90,         //  USER PARAMETER - Overtemperature, System Shudown When Exceeded (deg C)
        backlightSleepMode = 0,      //  USER PARAMETER - 0 = Never, 1 = 10secs, 2 = 5mins, 3 = 1hr, 4 = 6 hrs, 5 = 12hrs, 6 = 1 day, 7 = 3 days, 8 = 1wk, 9 = 1month
        telemCounterReset = 0,       //  USER PARAMETER - Reset Telem Data Every (0 = Never, 1 = Day, 2 = Week, 3 = Month, 4 = Year)
        mqttPort = 1883;

    bool
        disableFlashAutoLoad = false,
        MPPTMode = true,              //   USER PARAMETER - Enable MPPT algorithm, when disabled charger uses CC-CV algorithm
        outputMode = true,            //   USER PARAMETER - 0 = PSU MODE, 1 = Charger Mode
        enableWiFi = true,            //   USER PARAMETER - Enable WiFi Connection
        overrideFan = false,          //   USER PARAMETER - Fan always on
        fanStatus = false,            //   SYSTEM PARAMETER - Fan activity status (True = On, False = Off)
        enableDynamicCooling = false, //   USER PARAMETER - Enable for PWM cooling control
        enableFan = true,             //   USER PARAMETER - Enable Cooling Fan
        flashMemLoad = false,         // SYSTEM PARAMETER -
        batteryRelayState = false;

    float
        voltageBatteryMax = 14.5000, //   USER PARAMETER - Maximum Battery Charging Voltage (Output V)
        voltageBatteryMin = 10.8000, //   USER PARAMETER - Minimum Battery Charging Voltage (Output V)
        currentCharging = 5.0000,    //   USER PARAMETER - Maximum Charging Current (A - Output)
        energySavings = 0.0000,      // SYSTEM PARAMETER - Energy savings in fiat currency (Peso, USD, Euros or etc...)
        daysRunning = 0.0000;        // SYSTEM PARAMETER - Stores the total number of days the MPPT device has been running since last powered
    unsigned long
        currentRoutineMillis = 0, // SYSTEM PARAMETER -
        prevRoutineMillis = 0,    // SYSTEM PARAMETER -
        secondsElapsed = 0;       // SYSTEM PARAMETER -
    uint8_t relayPin;

    String
        mqttEndpoint,
        mqttServer,
        mqttUser,
        mqttPassword;

    void loadSettings();
    void saveConfig();
    void loadConfig();
    void batteryRelay();
};

#endif