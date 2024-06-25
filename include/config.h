#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>

#define FIRMWARE_VERSION "1.0"
#define USEOTA

typedef enum
{
    DISABLE,
    DISPLAY_ALL,
    ESSENTIAL_DATA,
    DISPLAY_NUMBER
} SERIAL_TELEMERTY_MODE;

typedef enum
{                // FOR ADS1015 12-BIT and ADS1115 16-BIT ADC MODEL
    G_TWOTHIRDS, // Gain: 2/3x  Range: +/- 6.144V
    G_ONE,       // Gain: 1x    Range: +/- 4.096V
    G_TWO        // Gain: 2x    Range: +/- 2.048V
} GAIN_SELECT;

typedef enum // Power Source
{
    USB_SOURCE,
    INPUT_SOURCE,
    BATTERY_SOURCE
} POWER_SOURCE;

typedef enum // PSU Nide
{
    PSU_MODE,
    CHARER_MODE
} OUTPUT_MODE;

typedef enum
{
    ROTARY_IDLE,
    ROTARY_CW,
    ROTARY_CCW
} ROTARY_STATE;

#define ADS1015 // Remove This tag if you use ADS1015
// #define ADS1115      //Remove This tag if you use ADS1115
#define OLED // Remove This tag if you use OLED
// #define LCD_CRYSTAL //Remove This tag if you use LCD Crystal

#define SERIAL_SPEED 500000

// I2S Address
#define ADS1X15_ADDRESS_1 0x48
#define ADS1X15_ADDRESS_2 0x49
#define OLED_ADDRESS 0x3C

// ESP32 Pin Usage
#define BACKFLOW_MOSFET 27
#define RELAY 26
#define BUCK_IN 33
#define BUCK_EN 32
#define FAN 16
#define ADC_ALERT_1 34
#define ADC_ALERT_2 25
#define TEMP_SENSOR 35
#define CLK 17
#define DT 18
#define SW 19

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BUTTON_LEFT 19
#define BUTTON_RIGHT 18
#define BUTTON_BACK 17
#define BUTTON_SELECT 23

// Constant Variable
#define NTC_RESISTANSE 10000.00
#define PWM_CHANEL 0
#define PWM_FREQUENCY 39000
#define PWM_RESOLUTION 11
#define PWM_MAX_DC 97.0000

// ADS1x15 Analog Pin Input
#define VOLTAGE_ANALOG_INPUT 0   // ADS1x15 Analog Input Pin Address 1
#define AMPERE_ANALOG_INPUT 1    // ADS1x15 Analog Input Pin Address 1
#define VOLTAGE_ANALOG_BATTERY 3 // ADS1x15 Analog Input Pin Address 1
#define AMPERE_ANALOG_BATTERY 2  // ADS1x15 Analog Input Pin Address 1
#define VOLTAGE_ANALOG_LOAD 2    // ADS1x15 Analog Input Pin Address 2
#define AMPERE_ANALOG_LOAD 3     // ADS1x15 Analog Input Pin Address 2

//  Constant Variable Editable
#define AVG_COUNT_CURRENT 5
#define AVG_COUNT_VOLTAGE 1
#define AVG_COUNT_TEMPERATURE 1
#define ADC_GAIN_SELECT G_TWO
#define SERIAL_TELE_MODE DISPLAY_ALL

// wifi

#define SSID_CONFIG_NAME "CITRANET MPPT"
#define SSID_CONFIG_PASSWROD "12345678"

// Mqtt
#define MQTT_SEND_DELAY 1000
#define CLIENT_ID "Arduino TEST"
#define CLIENT_NAME "test_arduino_1"
#define MQTT_GATE "test/"

#define SUBSCRIBE_RELAY MQTT_GATE CLIENT_NAME "/relay"
#define PUBLISH_SENSOR_TOPIC MQTT_GATE CLIENT_NAME "/sensor"
#define PUBLISH_CONFIG_TOPIC MQTT_GATE CLIENT_NAME "/config"

#endif