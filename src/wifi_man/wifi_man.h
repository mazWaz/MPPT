#ifndef WIFI_MAN
#define WIFI_MAN

#include "config.h"
#include <WiFiManager.h>
#include <time.h>
#include <stdio.h>

// enable OTA
#ifdef USEOTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

#include "./system_process/system_process.h"
#include "./sensor/sensor_reading.h"

class SystemProcess;
class SensorReading;

class WifiMan
{

public:
  WifiMan();
  void setup(SystemProcess *system_process, SensorReading *_sensor);
  void wifiMan();
  void resetWifi();
  void reseSettingtWifi();
  void setWebMqttConnection(bool state);
  String getSSID() { return wm.getWiFiSSID(); }
  void wifiOndemand() { wifiOndemandFlag = true; };

private:
  SystemProcess *systemProcess;
  SensorReading *sensor;

  void saveWifiCallback();
  void configModeCallback(WiFiManager *myWiFiManager);
  void saveParamCallback();
  void handleRoute();
  void bindServerCallback();
  void handlePreOtaUpdateCallback();
  void getTime();
  String getMonitoringHTML();

  String getCallbackParam(String name);

  WiFiManager wm;
  WiFiManagerParameter *custom_html;
  WiFiManagerParameter *mqttEndoint;
  WiFiManagerParameter *mqttServer;
  WiFiManagerParameter *mqttPort;
  WiFiManagerParameter *mqttUser;
  WiFiManagerParameter *mqttPassword;
  WiFiManagerParameter *mqttConnection;

  const char *modes[4] = {"NULL", "STA", "AP", "STA+AP"};

  unsigned long mtime = 0;

  bool
      ALLOWONDEMAND = true, // enable on demand
      mqttConnectionState = false,
      wifiOndemandFlag = false;
  int ONDDEMANDPIN = 0; // gpio for button
};

#endif