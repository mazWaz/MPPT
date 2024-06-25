#include "wifi_man.h"
WifiMan ::WifiMan()
{
}
void WifiMan::setup(SystemProcess *system_process, SensorReading *_sensor)
{
    systemProcess = system_process;
    sensor = _sensor;
    WiFi.mode(WIFI_STA);
    wm.setDebugOutput(true);
    wm.debugPlatformInfo();

    custom_html = new WiFiManagerParameter("<p style=\" font - weight: Bold;\">MQTT Setup</p>");
    mqttConnection = new WiFiManagerParameter("custom_input", "custom input", "", 64, "type='text' style='color: white;border:none !important;background-color: transparent;font-weight: Bold;font-size : 20px; cursor: default; outline: none;' readonly", WFM_NO_LABEL);
    mqttEndoint = new WiFiManagerParameter("mqtt_end_point", "Mqtt Endpoint", "", 64);
    mqttServer = new WiFiManagerParameter("mqtt_server", "Mqtt Server", "", 64);
    mqttPort = new WiFiManagerParameter("mqtt_port", "Mqtt Port", "1883", 64);
    mqttUser = new WiFiManagerParameter("mqtt_user", "Mqtt Username", "", 64);
    mqttPassword = new WiFiManagerParameter("mqtt_password", "Mqtt Password", "", 64, "type='password'");

    // callbacks
    wm.setAPCallback(std::bind(&WifiMan::configModeCallback, this, std::placeholders::_1));
    wm.setWebServerCallback(std::bind(&WifiMan::bindServerCallback, this));
    wm.setSaveConfigCallback(std::bind(&WifiMan::saveWifiCallback, this));
    wm.setSaveParamsCallback(std::bind(&WifiMan::saveParamCallback, this));
    wm.setPreOtaUpdateCallback(std::bind(&WifiMan::handlePreOtaUpdateCallback, this));

    // add all your parameters here
    wm.addParameter(custom_html);
    wm.addParameter(mqttConnection);
    wm.addParameter(mqttEndoint);
    wm.addParameter(mqttServer);
    wm.addParameter(mqttPort);
    wm.addParameter(mqttUser);
    wm.addParameter(mqttPassword);

    mqttServer->setValue(systemProcess->getMqttServer().c_str(), 64);
    mqttEndoint->setValue(systemProcess->getMqttEndPoint().c_str(), 64);
    mqttPort->setValue(String(systemProcess->getMqttPort()).c_str(), 64);
    mqttUser->setValue(systemProcess->getMqttUser().c_str(), 64);
    mqttPassword->setValue(systemProcess->getMqttPassword().c_str(), 64);

    const char *menuhtml = "<form action='/monitor' method='get'><button>Monitor</button></form><br/>\n";
    wm.setCustomMenuHTML(menuhtml);

    wm.setDarkMode(true);

    std::vector<const char *> menu = {"wifi", "info", "param", "custom", "close", "sep", "update", "restart", "exit"};
    wm.setMenu(menu); // custom menu, pass vector

    wm.setHostname(("WM_" + wm.getDefaultAPName()).c_str());

    wm.setConfigPortalBlocking(false);

    wm.setConfigPortalTimeout(120);

    wm.setBreakAfterConfig(true); // needed to use saveWifiCallback

    if (!wm.autoConnect())
    {
        delay(1000);
        wm.setConfigPortalTimeout(120);
        wm.startConfigPortal(SSID_CONFIG_NAME, SSID_CONFIG_PASSWROD);
    }

    wm.startWebPortal();
#ifdef USEOTA
    ArduinoOTA.begin();
#endif
}

void WifiMan::wifiMan()
{
#ifdef USEOTA
    ArduinoOTA.handle();
#endif

    if (!wm.getWebPortalActive())
    {
        wm.startWebPortal();
    }

    wm.process();

    // every 10 seconds
    if (millis() - mtime > 10000)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            getTime();
        }
        else
        {
            WiFi.begin();
            Serial.println("No Wifi");
        }

        mtime = millis();
    }

    if (wifiOndemandFlag)
    {
        wm.setConfigPortalTimeout(140);

        if (!wm.startConfigPortal(SSID_CONFIG_NAME, SSID_CONFIG_PASSWROD))
        {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
        }
        wifiOndemandFlag = false;
    }
}

void WifiMan::saveWifiCallback()
{
    Serial.println("[CALLBACK] saveCallback fired");
}

void WifiMan::configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("[CALLBACK] configModeCallback fired");
    // myWiFiManager->setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
    // Serial.println(WiFi.softAPIP());
    // if you used auto generated SSID, print it
    // Serial.println(myWiFiManager->getConfigPortalSSID());
    //
    // esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);
}

void WifiMan::saveParamCallback()
{
    systemProcess->saveParamMqtt(
        getCallbackParam("mqtt_end_point"),
        getCallbackParam("mqtt_server"),
        getCallbackParam("mqtt_port"),
        getCallbackParam("mqtt_user"),
        getCallbackParam("mqtt_password"));
}

void WifiMan::handleRoute()
{
    Serial.println(getMonitoringHTML());
    wm.server->send(200, "text/html", getMonitoringHTML());
}

void WifiMan::bindServerCallback()
{
    std::function<void()> handleRoute = std::bind(&WifiMan::handleRoute, this);
    wm.server->on("/monitor", handleRoute); // this is now crashing esp32 for some reason
                                            // wm.server->on("/info",handleRoute); // you can override wm!
}

void WifiMan::getTime()
{
    int tz = -5;
    int dst = 0;
    time_t now = time(nullptr);
    unsigned timeout = 5000; // try for timeout
    unsigned start = millis();
    configTime(tz * 3600, dst * 3600, "0.id.pool.ntp.org", "1.id.pool.ntp.org");
    Serial.print("Waiting for NTP time sync: ");
    while (now < 8 * 3600 * 2)
    { // what is this ?
        delay(100);
        Serial.print(".");
        now = time(nullptr);
        if ((millis() - start) > timeout)
        {
            Serial.println("\n[ERROR] Failed to get NTP time.");
            return;
        }
    }
    Serial.println("");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));
}

String WifiMan::getCallbackParam(String name)
{

    if (wm.server->hasArg(name))
    {
        return wm.server->arg(name);
    }
    return "";
}

void WifiMan::handlePreOtaUpdateCallback()
{
    Update.onProgress([](unsigned int progress, unsigned int total)
                      { Serial.printf("CUSTOM Progress: %u%%\r", (progress / (total / 100))); });
}

void WifiMan::setWebMqttConnection(bool state)
{
    if (state)
    {
        mqttConnection->setValue("Mqtt Connected", 64);
    }
    else
    {
        mqttConnection->setValue("Mqtt Not Connected", 64);
    }
}

void WifiMan::resetWifi()
{
    wm.resetSettings();
}

void WifiMan::reseSettingtWifi()
{
    wm.resetSettings();
    wm.erase();
}

String WifiMan::getMonitoringHTML()
{
    String html = "";
    html += "<style>\n"
            "  body {\n"
            "    background-color: #060606;\n"
            "    color: #ffffff;\n"
            "  }\n"
            "  table {\n"
            "    padding-left: 0px;\n"
            "  }\n"
            "\n"
            "  th,\n"
            "  td {\n"
            "    padding-left: 15px;\n"
            "    font-size: 20px;\n"
            "  }\n"
            "  tr td:first-child {\n"
            "    font-weight: bold;\n"
            "  }\n"
            "  button,\n"
            "  input[type=\"button\"],\n"
            "  input[type=\"submit\"] {\n"
            "    border-radius: 0.3rem;\n"
            "    cursor: pointer;\n"
            "    border: 0;\n"
            "    background-color: #1fa3ec;\n"
            "    color: #fff;\n"
            "    line-height: 2.4rem;\n"
            "    font-size: 1.2rem;\n"
            "    width: 20%;\n"
            "  }\n"
            "</style>\n"
            "\n"
            "<body>\n"
            "  <div style=\"text-align: center\">\n"
            "    <p style=\"font-size: 40px; font-weight: bold\">Monitoring</p>\n"
            "    <table style=\"margin: 0 auto\">\n"
            "      <tr>\n"
            "        <td><b>Input Source</b></td>\n"
            "        <td>Battery</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Main Power</td>\n"
            "        <td>Yes</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Device Temperature</td>\n"
            "        <td>";
    html += sensor->getTemperature();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Voltage Input</td>\n"
            "        <td>";
    html += sensor->getVoltageInput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Current Input</td>\n"
            "        <td>";
    html += sensor->getCurrentInput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Power Input</td>\n"
            "        <td>";
    html += sensor->getPowerInput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Wh Input</td>\n"
            "        <td>";
    html += sensor->getWhInput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Kwh Input</td>\n"
            "        <td>";
    html += sensor->getKwhInput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Mwh Input</td>\n"
            "        <td>";
    html += sensor->getMWhInput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Voltage Battery</td>\n"
            "        <td>";
    html += sensor->getVoltageBattery();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Current Battert</td>\n"
            "        <td>";
    html += sensor->getCurrentBattery();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Power Battery</td>\n"
            "        <td>";
    html += sensor->getPowerBattery();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Percentage Battery</td>\n"
            "        <td>";
    html += sensor->getBattPercent();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Voltage Load</td>\n"
            "        <td>";
    html += sensor->getVoltageLoad();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Current Load</td>\n"
            "        <td>";
    html += sensor->getCurrentLoad();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Power Load</td>\n"
            "        <td>";
    html += sensor->getPowerLoad();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Wh Load</td>\n"
            "        <td>";
    html += sensor->getWhOutput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Kwh Load</td>\n"
            "        <td>";
    html += sensor->getKwhOutput();
    html += "</td>\n"
            "      </tr>\n"
            "      <tr>\n"
            "        <td>Mwh Load</td>\n"
            "        <td>";
    html += sensor->getMWhOutput();
    html +=
        "</td>\n"
        "      </tr>\n"
        "    </table>\n"
        "    <button id=\"relay\">RELAY</button>\n"
        "    <script>\n"
        "      const toggleDarkModeBtn = document.getElementById(\"relay\");\n"
        "      const body = document.body;\n"
        "\n"
        "      toggleDarkModeBtn.addEventListener(\"click\", function () {\n"
        "        console.log(\"aosdjjwd\");\n"
        "      });\n"
        "    </script>\n"
        "  </div>\n"
        "</body>\n";
    return html;
}