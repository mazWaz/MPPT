#include "system_process.h"

SystemProcess::SystemProcess(uint8_t fan_pin)
{
    fan = fan_pin;
}

void SystemProcess::setup(SensorReading *_sensor, WifiMan *_wifiMan, uint8_t _relay)
{
    sensor = _sensor;
    wifiMan = _wifiMan;
    relayPin = _relay;

    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);

    if (!SPIFFS.begin())
    {
        // Serial.println("Failed to mount FS");
        return;
    }

    pinMode(fan, OUTPUT);

    if (!disableFlashAutoLoad)
    {
        loadConfig();

        if (flashMemLoad == 1)
        {
            loadSettings();
        }
    }
}

void SystemProcess::loadConfig()
{
    if (SPIFFS.exists("/config.json"))
    {

        File file = SPIFFS.open("/config.json", "r");
        if (file)
        {
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, file);
            if (error)
            {
                // Serial.println(F("Failed to read file, using default configuration"));
            }

            // Set To Stuct
            config.mpptMode = doc["mpptMode"];
            config.outputMode = doc["outputMode"];
            config.voltageBatteryMax = doc["voltageBatteryMax"];
            config.voltageBatteryMin = doc["voltageBatteryMin"];
            config.currentCharging = doc["currentCharging"];
            config.enableFan = doc["enableFan"];
            config.temperatureFan = doc["temperatureFan"];
            config.temperatureMax = doc["temperatureMax"];
            config.saveEnableWifi = doc["saveEnableWifi"];
            config.backlightSleepMode = doc["backlightSleepMode"];
            config.autoLoadSetting = doc["autoLoadSetting"];
            flashMemLoad = doc["autoLoadSetting"];

            strlcpy(config.mqttEndpoint, doc["mqttEndpoint"], sizeof(config.mqttEndpoint));
            strlcpy(config.mqttServer, doc["mqttServer"], sizeof(config.mqttServer));
            config.mqttPort = doc["mqttPort"];
            strlcpy(config.mqttUser, doc["mqttUser"], sizeof(config.mqttUser));
            strlcpy(config.mqttPassword, doc["mqttPassword"], sizeof(config.mqttPassword));
        }
        file.close();
    }
}

void SystemProcess::saveConfig()
{
    if (SPIFFS.exists("/config.json"))
    {
        File file = SPIFFS.open("/config.json", "w");
        if (file)
        {
            StaticJsonDocument<512> doc;

            doc["mpptMode"] = config.mpptMode;
            doc["outputMode"] = config.outputMode;
            doc["voltageBatteryMax"] = config.voltageBatteryMax;
            doc["voltageBatteryMin"] = config.voltageBatteryMin;
            doc["currentCharging"] = config.currentCharging;
            doc["enableFan"] = config.enableFan;
            doc["temperatureFan"] = config.temperatureFan;
            doc["temperatureMax"] = config.temperatureMax;
            doc["saveEnableWifi"] = config.saveEnableWifi;
            doc["backlightSleepMode"] = config.backlightSleepMode;
            doc["autoLoadSetting"] = config.autoLoadSetting;
            doc["mqttEndpoint"] = config.mqttEndpoint;
            doc["mqttServer"] = config.mqttServer;
            doc["mqttPort"] = config.mqttPort;
            doc["mqttUser"] = config.mqttUser;
            doc["mqttPassword"] = config.mqttPassword;

            if (serializeJson(doc, file) == 0)
            {
                // Serial.println(F("Failed to write to file"));
            }
        }
        file.close();
        loadSettings();
    }
    else
    {
        // Serial.println(F("file not found................."));
    }
}

void SystemProcess::loadSettings()
{
    MPPTMode = config.mpptMode;
    outputMode = config.outputMode;
    voltageBatteryMax = config.voltageBatteryMax;
    voltageBatteryMin = config.voltageBatteryMin;
    currentCharging = config.currentCharging;
    enableFan = config.enableFan;
    temperatureFan = config.temperatureFan;
    temperatureMax = config.temperatureMax;
    enableWiFi = config.saveEnableWifi;
    backlightSleepMode = config.backlightSleepMode;
    mqttEndpoint = config.mqttEndpoint;
    flashMemLoad = config.autoLoadSetting;
    mqttServer = config.mqttServer;
    mqttPort = config.mqttPort;
    mqttUser = config.mqttUser;
    mqttPassword = config.mqttPassword;
}

void SystemProcess::factoryReset()
{
    config.mpptMode = false;
    config.outputMode = true;
    config.voltageBatteryMax = 12.5;
    config.voltageBatteryMin = 11.5;
    config.currentCharging = 0.5;
    config.enableFan = true;
    config.temperatureFan = 60;
    config.temperatureMax = 90;
    config.saveEnableWifi = true;
    config.backlightSleepMode = 0;
    config.autoLoadSetting = true;
    mqttEndpoint = "/test/test_arduino_1";
    mqttServer = "";
    mqttPort = 1883;
    mqttUser = "";
    mqttPassword = "";
    wifiMan->reseSettingtWifi();
    saveConfig();
    loadSettings();
    ESP.restart();
}

void SystemProcess::saveMPPTMode(bool mppt_value)
{
    config.mpptMode = mppt_value; // STORE: Algorithm
    saveConfig();                 // Saves setting changes to flash memory
}
void SystemProcess::saveOutputMode(bool output_mode)
{
    config.outputMode = output_mode; // STORE: Charge/PSU Mode Selection
    saveConfig();                    // Saves setting changes to flash memory
}
void SystemProcess::saveVoltageBatteryMax(float voltage_batt_max)
{
    config.voltageBatteryMax = voltage_batt_max; // STORE: Maximum Battery Voltage (gets whole number)
    saveConfig();                                // Saves setting changes to flash memory
}
void SystemProcess::saveVoltageBatteryMin(float voltage_batt_min)
{
    config.voltageBatteryMin = voltage_batt_min; // STORE: Minimum Battery Voltage (gets whole number)
    saveConfig();                                // Saves setting changes to flash memory
}
void SystemProcess::saveCurrentCharging(float current_charging)
{
    config.currentCharging = current_charging;
    saveConfig();                   // Saves setting changes to flash memory
    conv1 = current_charging * 100; // STORE: Charging Current
    conv2 = conv1 % 100;
}
void SystemProcess::saveEnableFan(bool enable_fan)
{
    config.enableFan = enable_fan; // STORE: Fan Enable
    saveConfig();                  // Saves setting changes to flash memory
}
void SystemProcess::saveTemperatureFan(int temperature_fan)
{
    config.temperatureFan = temperature_fan; // STORE: Fan Temp
    saveConfig();                            // Saves setting changes to flash memory
}
void SystemProcess::saveTemperatureMax(int temperature_max)
{
    config.temperatureMax = temperature_max;
    saveConfig(); // Saves setting changes to flash memory
}
void SystemProcess::saveEnableWifi(bool enable_wifi)
{
    config.saveEnableWifi = enable_wifi; // STORE: Shutdown Temp
    saveConfig();                        // Saves setting changes to flash memory
}
void SystemProcess::saveBacklightSleepMode(int backlight_sleep_mode)
{
    config.backlightSleepMode = backlight_sleep_mode; // STORE: LCD backlight sleep timer
    saveConfig();                                     // Saves setting changes to flash memory
}

void SystemProcess::saveAutoloadSettings()
{
    config.autoLoadSetting = flashMemLoad; // STORE: Enable autoload
    saveConfig();                          // Saves setting changes to flash memory
}

void SystemProcess::saveParamMqtt(String mqtt_end_point, String mqtt_server, String mqtt_port, String mqtt_user, String mqtt_password)
{
    strlcpy(config.mqttEndpoint, mqtt_end_point.c_str(), sizeof(config.mqttEndpoint));
    strlcpy(config.mqttServer, mqtt_server.c_str(), sizeof(config.mqttServer));
    strlcpy(config.mqttUser, mqtt_user.c_str(), sizeof(config.mqttUser));
    strlcpy(config.mqttPassword, mqtt_password.c_str(), sizeof(config.mqttPassword));
    config.mqttPort = mqtt_port.toInt();
    saveConfig();
}

void SystemProcess::fanSystem()
{

    if (enableFan)
    {
        if (!enableDynamicCooling) // STATIC PWM COOLING MODE (2-PIN FAN - no need for hysteresis, temp data only refreshes after 'avgCountTS' or every 500 loop cycles)
        {
            if (overrideFan)
            {
                fanStatus = true; // Force on fan
            }
            if (sensor->getTemperature() >= temperatureFan)
            {

                fanStatus = true; // Turn on fan when set fan temp reached
            }
            if (sensor->getTemperature() < temperatureFan - 2)
            {
                fanStatus = false;
            }                             // Turn off fan when set fan temp reached
            digitalWrite(fan, fanStatus); // Send a digital signal to the fan MOSFET
        }
        else
        {
            // DYNAMIC PWM COOLING MODE (3-PIN FAN - coming soon)
        }
    }
    else
    {
        digitalWrite(fan, LOW);
    } // Fan Disabled
}

void SystemProcess::batteryRelay()
{
    if (sensor->getVoltageBattery() > voltageBatteryMax && outputMode)
    {
        digitalWrite(relayPin, LOW);
        batteryRelayState = true;
    }
    else
    {
        digitalWrite(relayPin, HIGH);
        batteryRelayState = false;
    }
}

void SystemProcess::setRelay(bool relayState)
{
    if (relayState && !batteryRelayState)
    {
        digitalWrite(relayPin, HIGH);
    }
    else
    {
        digitalWrite(relayPin, LOW);
    }
}

void SystemProcess::timeCounting()
{
    secondsElapsed = millis() / 1000; // Gets the time in seconds since the was turned on  and active
    currentRoutineMillis = millis();
    if (currentRoutineMillis - prevRoutineMillis >= millisRoutineInterval)
    {

        setDaysRunning(timeOn / (86400.000 * (1000.000 / millisRoutineInterval))); // Compute for days running (86400s*(1000/interval))
        setTimeOn(timeOn++);                                                       // Increment time counter
    }
}

void SystemProcess::resetVariables()
{
    secondsElapsed = 0;
    energySavings = 0;
    daysRunning = 0;
    timeOn = 0;
}