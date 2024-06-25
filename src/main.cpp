#include "Config.h"
#include "./sensor/sensor_reading.h"
#include "./system_process/system_process.h"
#include "./device_protection/device_protection.h"
#include "./charging_algorithm/charging_algorithm.h"
#include "./oled_display/oled.h"
#include "./encoder/encoder.h"
#include "./mqtt/mqtt.h"
#include "./wifi_man/wifi_man.h"

#include "./task/task.h"

SemaphoreHandle_t readSensorMutex = NULL; // Create a mutex object

SensorReading *sensor;
SystemProcess *systemProcess;
DeviceProtection *deviceProtection;
ChargingAlgorithm *chargingAlgorithm;
Encoder *encoder;
OledDisplay *oledDisplay;
WifiMan *wifiMan;
Mqtt *mqtt;

void setupDisplay()
{
#ifdef OLED

#endif

#ifdef LCD_CRYSTAL

#endif
}

void setup()
{
  Serial.begin(SERIAL_SPEED);
  pinMode(RELAY, OUTPUT);

  systemProcess = new SystemProcess(FAN);
  sensor = new SensorReading(ADS1X15_ADDRESS_1, ADS1X15_ADDRESS_2, TEMP_SENSOR);
  deviceProtection = new DeviceProtection(BACKFLOW_MOSFET);
  chargingAlgorithm = new ChargingAlgorithm(BUCK_EN, BUCK_IN);
  oledDisplay = new OledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_ADDRESS);
  encoder = new Encoder(CLK, DT, SW);
  wifiMan = new WifiMan();
  mqtt = new Mqtt();

  systemProcess->setup(sensor, wifiMan, RELAY);
  sensor->setup(systemProcess, deviceProtection, chargingAlgorithm);
  deviceProtection->setup(systemProcess, sensor);
  chargingAlgorithm->setup(systemProcess, sensor, deviceProtection);
  oledDisplay->setup(encoder, sensor, systemProcess, deviceProtection, wifiMan);
  wifiMan->setup(systemProcess, sensor);
  mqtt->setup(systemProcess, wifiMan, sensor);

  readSensorMutex = xSemaphoreCreateMutex(); // crete a mutex object

  static TaskArg taskArg = {
      .sensor = sensor,
      .systemProcess = systemProcess,
      .deviceProtection = deviceProtection,
      .chargingAlgorithm = chargingAlgorithm,
      .encoder = encoder,
      .oledDisplay = oledDisplay,
      .wifiMan = wifiMan,
      .mqtt = mqtt,
      .readSensorMutex = readSensorMutex,
  };

  xTaskCreatePinnedToCore(&wifiTask, "wifi", 65536, &taskArg, 5, NULL, 0);                   // Wifi Core
  xTaskCreatePinnedToCore(&mqttTask, "mqtt", 8192, &taskArg, 4, NULL, 0);                    // Wifi Core
  xTaskCreatePinnedToCore(&systemProcessTask, "systemProcess", 16384, &taskArg, 3, NULL, 0); // Wifi Core
  xTaskCreatePinnedToCore(&serialTelemetryTask, "serial", 8192, &taskArg, 2, NULL, 0);       // Wifi Core

  xTaskCreatePinnedToCore(&sensorReadingTask, "sensorReading", 8192, &taskArg, 5, NULL, 1);           // Main Core
  xTaskCreatePinnedToCore(&deviceProtectionTask, "deviceProtection", 8192, &taskArg, 3, NULL, 1);     // Main Core
  xTaskCreatePinnedToCore(&charginAlgorithmTask, "charginAlgorithmTask", 8192, &taskArg, 4, NULL, 1); // Main Core
  xTaskCreatePinnedToCore(&encoderTask, "encoder", 8192, &taskArg, 2, NULL, 1);                       // Main Core
  xTaskCreatePinnedToCore(&oledTask, "oled", 8192, &taskArg, 1, NULL, 0);                             // Main Core
}

void loop()
{
  vTaskDelete(NULL);
}