#include "oled.h"

OledDisplay::OledDisplay(uint8_t scrren_width, uint8_t screen_height, uint8_t oled_address)
{
    display = Adafruit_SSD1306(scrren_width, screen_height, &Wire);
    oledAddress = oled_address;
};

void OledDisplay::setup(Encoder *encoders, SensorReading *sensors, SystemProcess *system_process, DeviceProtection *device_protections, WifiMan *_wifiMan)
{
    encoder = encoders;
    sensor = sensors;
    systemProcess = system_process;
    deviceProtection = device_protections;
    wifiMan = _wifiMan;

    display.begin(SSD1306_SWITCHCAPVCC, oledAddress);
    display.clearDisplay();
    display.setTextColor(WHITE);
    splashScreen();
    delay(5000);
}

// void OledDisplay::displayAwake()
// {
//     display.ssd1306_command(SSD1306_DISPLAYON);
//     prevLCDBackLMillis = millis();
// }

void OledDisplay::displayMenu()
{
    // byte encoderSinggle = encoder->singglePush();

    switch (menuPointer)
    {
    case MAIN_MENUS:
        switch (encoder->getPosition()) // main Menu Control
        {
        case INPUT_MENU:
            mainMenuInput();
            break;
        case BATTERY_MENU:
            if (systemProcess->getOutputMode())
            {
                mainMenuBattery();
            }
            else
            {
                encoder->setPosition(OUTPUT_MENU);
            }

            break;
        case OUTPUT_MENU:
            mainMenuOutput();
            break;

        case SETTING_MENU:
            mainMenuSettings();
            if (encoder->singglePush())
            {
                menuPointer++;
                encoder->setPosition(0);
            }
            break;

        default:
            if (encoder->getPosition() < 0)
            {
                encoder->setPosition(SETTING_MENU);
            }
            else
            {
                encoder->setPosition(0);
            }

            break;
        }
        break;

    case SETTING_MENUS:
        switch (encoder->getPosition())
        {
        case INPUT_MODE:
            settingInputMode();
            break;
        case CHARGING_MODE:
            settingChargingMode();
            break;
        case BATTERY_MAX:
            settingBatteryMax();
            break;
        case BATTERY_MIN:
            settingBatteryMin();
            break;
        case CURRENT:
            settingChargingCurrent();
            break;
        case FAN_SETTING:
            settingCollingFan();
            break;
        case FAN_TEMPERATURE:
            settingFanTemperature();
            break;
        case SHUTDOWN_TEMPERATURE:
            settingShutdownTemperature();
            break;
        case WIFI_SETTING:
            settingWifi();
            break;
        case FACTORY_RESET:
            settingFactoryReset();
            break;
        case EXIT:
            exit();
            if (encoder->singglePush())
            {
                menuPointer--;
                encoder->setPosition(0);
            }
            break;
        default:
            if (encoder->getPosition() < 0)
            {
                encoder->setPosition(EXIT);
            }
            else
            {
                encoder->setPosition(0);
            }
            break;
        }
        break;

    default:
        menuPointer = 0;
        break;
    }
}

void OledDisplay::splashScreen()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(40, 15);
    display.println("MPPT");
    display.setCursor(2, 30);
    display.setTextSize(1);
    display.println("INITIALIZED");
    display.setCursor(20, 45);
    display.setTextSize(1);
    display.setFont(NULL);
    display.print("FRIMWARE V");
    display.println(FIRMWARE_VERSION);
    display.setCursor(20, 55);
    display.setTextSize(1);
    display.setFont(&FreeSerifItalic9pt7b);
    display.println("Citranet Solo");
    display.display();
}

void OledDisplay::mainMenuInput()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(35, 10);
    display.println("INPUT");
    display.setFont(NULL);
    display.setCursor(0, 16);
    display.print("Voltage     : ");
    display.print(sensor->getVoltageInput());
    display.println("V");
    display.setCursor(0, 26);
    display.print("Ampere      : ");
    display.print(sensor->getCurrentInput());
    display.println("A");
    display.setCursor(0, 37);
    display.print("Power       : ");
    display.print(sensor->getPowerInput());
    display.println("W");
    display.setCursor(0, 47);
    display.print("Energy      : ");
    if (sensor->getWhInput() < 10)
    {
        display.print(sensor->getWhInput(), 3);
        display.println("Wh");
    }
    else if (sensor->getWhInput() < 100)
    {
        display.print(sensor->getWhInput(), 2);
        display.println("Wh");
    }
    else if (sensor->getWhInput() < 1000)
    {
        display.print(sensor->getWhInput(), 1);
        display.println("Wh");
    }
    else if (sensor->getWhInput() < 10000)
    {
        display.print(sensor->getKwhInput(), 2);
        display.println("kWh");
    }
    else if (sensor->getWhInput() < 100000)
    {
        display.print(sensor->getKwhInput(), 1);
        display.println("kWh");
    }
    else if (sensor->getWhInput() < 1000000)
    {
        display.print(sensor->getKwhInput(), 0);
        display.println("kWh");
    }
    else if (sensor->getWhInput() < 10000000)
    {
        display.print(sensor->getMWhInput(), 1);
        display.println("MWh");
    }
    else if (sensor->getWhInput() > 10000000)
    {
        display.print(sensor->getMWhInput(), 0);
        display.println("MWh");
    }
    display.drawBitmap(5, -5, logoInput, 24, 24, 1);
    display.display();
}

void OledDisplay::mainMenuOutput()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(35, 10);
    display.println("OUTPUT");
    display.setFont(NULL);
    display.setCursor(0, 16);
    display.print("Voltage     : ");
    display.print(sensor->getVoltageLoad());
    display.println("V");
    display.setCursor(0, 26);
    display.print("Ampere      : ");
    display.print(sensor->getCurrentLoad());
    display.println("A");
    display.setCursor(0, 37);
    display.print("Power       : ");
    display.print(sensor->getPowerLoad());
    display.println("W");
    display.setCursor(0, 47);
    display.print("Energy      : ");
    if (sensor->getWhOutput() < 10)
    {
        display.print(sensor->getWhOutput(), 3);
        display.println("Wh");
    }
    else if (sensor->getWhOutput() < 100)
    {
        display.print(sensor->getWhOutput(), 2);
        display.println("Wh");
    }
    else if (sensor->getWhOutput() < 1000)
    {
        display.print(sensor->getWhOutput(), 1);
        display.println("Wh");
    }
    else if (sensor->getWhOutput() < 10000)
    {
        display.print(sensor->getKwhOutput(), 2);
        display.println("kWh");
    }
    else if (sensor->getWhOutput() < 100000)
    {
        display.print(sensor->getKwhOutput(), 1);
        display.println("kWh");
    }
    else if (sensor->getWhOutput() < 1000000)
    {
        display.print(sensor->getKwhOutput(), 0);
        display.println("kWh");
    }
    else if (sensor->getWhOutput() < 10000000)
    {
        display.print(sensor->getMWhOutput(), 1);
        display.println("MWh");
    }
    else if (sensor->getWhOutput() > 10000000)
    {
        display.print(sensor->getMWhOutput(), 0);
        display.println("MWh");
    }
    display.drawBitmap(5, -5, logoOutput, 24, 24, 1);
    display.display();
}

void OledDisplay::mainMenuBattery()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(35, 10);
    display.println("BATTERY");
    display.drawBitmap(5, -5, logoBattery, 24, 24, 1);
    if (deviceProtection->getBNC() == 0)
    {

        display.setFont(NULL);
        display.setCursor(0, 16);
        display.print("Voltage     : ");
        display.print(sensor->getVoltageBattery());
        display.println("V");
        display.setCursor(0, 27);
        display.print("Ampere      : ");
        display.print(sensor->getCurrentBattery());
        display.println("A");
        display.setCursor(0, 37);
        display.print("Percentage  : ");
        display.print(sensor->getBattPercent());
        display.println("%");
        display.drawRoundRect(5, 49, 116, 15, 3, 1);
        display.fillRoundRect(120, 52, 5, 9, 3, 1);
        // 116Max

        display.fillRoundRect(5, 50,
                              map(sensor->getBattPercent(), 0, 100, 0, 116),
                              13, 3, 1);
        display.display();
    }
    else
    {
        display.setTextSize(1);
        display.setFont(NULL);
        display.setCursor(33, 29);
        display.println("NO BATTERY");
        display.display();
    }
}

void OledDisplay::mainMenuSettings()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(17, 10);
    display.println("SETTINGS");
    display.drawBitmap(50, 30, logoSettings, 24, 24, 1);
    display.display();
}

void OledDisplay::settingInputMode()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.drawBitmap(0, 0, logoSupplyAlorithm, 24, 24, 1);
    display.setFont(NULL);
    if (selectedPush())
    {
        if (!initValue)
        {
            encoder->setValue(systemProcess->getMPPTMode());
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();
        if (encoder->getValue() > 1)
        {
            encoder->setValue(0);
        }

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }

        systemProcess->setMPPTMode(encoder->getValue());
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveMPPTMode(encoder->getValue());
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }

    display.setCursor(30, 8);
    display.println("SUPPLY ALGORITHM");
    if (systemProcess->getMPPTMode())
    {
        display.setCursor(35, 34);
        display.println("Solar Panel");
        display.setCursor(30, 48);
        display.println("MPPT + CC-CVV");
    }
    else
    {
        display.setCursor(30, 34);
        display.println("Power Supply");
        display.setCursor(36, 48);
        display.println("CC-CV Only");
    }

    display.display();
}

void OledDisplay::settingChargingMode()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.drawBitmap(0, 0, logoChargingMode, 24, 24, 1);
    display.setFont(NULL);
    if (selectedPush())
    {
        if (!initValue)
        {
            encoder->setValue(systemProcess->getOutputMode());
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();
        if (encoder->getValue() > 1)
        {
            encoder->setValue(0);
        }

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }

        systemProcess->setOutputMode(encoder->getValue());
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveOutputMode(encoder->getValue());
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }

    display.setCursor(35, 8);
    display.println("CHARGING MODE");
    display.setTextSize(2);
    display.setCursor(18, 40);
    display.setTextSize(2);
    if (systemProcess->getOutputMode())
    {
        display.println("BMS MODE");
    }
    else
    {
        display.println("PSU MODE");
    }

    display.display();
}

void OledDisplay::settingBatteryMax()
{
    display.clearDisplay();
    display.drawBitmap(5, -5, logoBatteryMaxMin, 24, 24, 1);
    if (selectedPush())
    {
        if (!initValue)
        {

            encoder->setValue(systemProcess->getVoltageBatteryMax() * 10);
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }
        if (sensor->getVoltageInput() < (float(encoder->getValue()) / 10) + 1)
        {
            encoder->setValue(systemProcess->getVoltageBatteryMax() * 10);
        }
        systemProcess->setVoltageBatteryMax(float(encoder->getValue()) / 10);
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveVoltageBatteryMax(float(encoder->getValue()) / 10);
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }
    display.setTextSize(1);
    display.setCursor(40, 0);
    display.println("BATTERY MAX");
    display.setCursor(26, 40);
    display.setTextSize(2);
    display.print(systemProcess->getVoltageBatteryMax());
    display.println("V");
    display.display();
}
void OledDisplay::settingBatteryMin()
{
    display.clearDisplay();
    display.drawBitmap(5, -5, logoBatteryMaxMin, 24, 24, 1);
    if (selectedPush())
    {
        if (!initValue)
        {

            encoder->setValue(systemProcess->getVoltageBatteryMin() * 10);
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }
        if (systemProcess->getVoltageBatteryMax() < (float(encoder->getValue()) / 10) + 1)
        {
            encoder->setValue(systemProcess->getVoltageBatteryMin() * 10);
        }
        systemProcess->setvoltageBatteryMin(float(encoder->getValue()) / 10);
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveVoltageBatteryMin(float(encoder->getValue()) / 10);
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }
    display.setTextSize(1);
    display.setCursor(40, 0);
    display.println("BATTERY MIN");
    display.setCursor(26, 40);
    display.setTextSize(2);
    display.print(systemProcess->getVoltageBatteryMin());
    display.println("V");
    display.display();
}

void OledDisplay::settingChargingCurrent()
{
    display.clearDisplay();
    display.drawBitmap(5, -5, currentLogo, 24, 24, 1);
    if (selectedPush())
    {
        if (!initValue)
        {

            encoder->setValue(systemProcess->getCurrentCharging() * 10);
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }
        systemProcess->setCurrentCharging(float(encoder->getValue()) / 10);
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveCurrentCharging(float(encoder->getValue()) / 10);
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }
    display.setTextSize(1);
    display.setCursor(40, 0);
    display.println("CURRENT");
    display.setCursor(26, 40);
    display.setTextSize(2);
    display.print(systemProcess->getCurrentCharging());
    display.println("A");
    display.display();
}

void OledDisplay::settingCollingFan()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.drawBitmap(0, 0, fanLogo, 24, 24, 1);
    display.setFont(NULL);
    if (selectedPush())
    {
        if (!initValue)
        {
            encoder->setValue(systemProcess->getFanStatus());
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();
        if (encoder->getValue() > 1)
        {
            encoder->setValue(0);
        }

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }

        systemProcess->setFanStatus(encoder->getValue());
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveEnableFan(encoder->getValue());
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }

    display.setCursor(35, 8);
    display.println("ENABLE FAN");
    display.setTextSize(2);
    display.setCursor(18, 40);
    display.setTextSize(2);
    if (systemProcess->getFanStatus())
    {
        display.println("ENABLE");
    }
    else
    {
        display.println("DISABLE");
    }

    display.display();
}

void OledDisplay::settingFanTemperature()
{
    display.clearDisplay();
    display.drawBitmap(5, -5, thermometerLogo, 24, 24, 1);
    if (selectedPush())
    {
        if (!initValue)
        {

            encoder->setValue(systemProcess->getTemperatureFan());
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }

        systemProcess->setTemperatureFan(encoder->getValue());
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveTemperatureFan(encoder->getValue());
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.println("FAN TRIGGER");
    display.setCursor(20, 10);
    display.println("TEMPERATURE");
    display.setCursor(26, 40);
    display.setTextSize(2);
    display.print(systemProcess->getTemperatureFan());
    display.print((char)247);
    display.display();
}

void OledDisplay::settingShutdownTemperature()
{
    display.clearDisplay();
    display.drawBitmap(5, -5, shutdownLogo, 24, 24, 1);
    if (selectedPush())
    {
        if (!initValue)
        {

            encoder->setValue(systemProcess->getTemperatureMax());
            initValue = true;
        }

        saveTemp = true;
        encoder->holdPosition();

        if (millis() - blinking > 750)
        {
            blinking = millis();
        }
        else
        {
            display.drawRect(5, 26, 119, 38, 1);
        }

        systemProcess->setTemperatureMax(encoder->getValue());
    }
    else
    {
        if (saveTemp)
        {
            systemProcess->saveTemperatureMax(encoder->getValue());
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
        display.drawRect(5, 26, 119, 38, 1);
    }
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.println("SHUTDOWN");
    display.setCursor(20, 10);
    display.println("TEMPERATURE");
    display.setCursor(26, 40);
    display.setTextSize(2);
    display.print(systemProcess->getTemperatureMax());
    display.print((char)247);
    display.display();
}

void OledDisplay::settingWifi()
{
    display.clearDisplay();
    if (selectedPush())
    {
        saveTemp = true;
        encoder->holdPosition();
        if (!initValue)
        {
            wifiMan->wifiOndemand();
            initValue = true;
        }
        delay(2000);
        display.setCursor(0, 10);
        display.setTextSize(2);
        display.println(WiFi.softAPSSID());
        display.println(WiFi.softAPIP());
        if (WiFi.softAPSSID().indexOf("ESP_") >= 0)
        {
            selectedButton = false;
            encoder->releasePosition();
            encoder->setPosition(0);
            menuPointer = 0;
        }
    }
    else
    {
        initValue = false;
        display.setTextSize(2);
        display.setCursor(0, 10);
        display.println("WiFi");
        display.println("Setting");
    }
    display.display();
}

void OledDisplay::settingFactoryReset()
{
    display.clearDisplay();

    if (selectedPush())
    {
        saveTemp = true;
        encoder->holdPosition();
        display.setTextSize(2);
        display.setCursor(25, 0);
        display.println("ARE YOU");
        display.setCursor(35, 20);
        display.print("SURE?");

        switch (encoder->getValue())
        {
        case 0:
            display.setCursor(50, 50);
            display.println("NO");
            break;
        case 1:
            display.setCursor(45, 50);
            display.println("YES");
            break;
        case 2:
            encoder->setValue(1);
            break;
        default:
            encoder->setValue(0);
            break;
        }
        if (encoder->getValue() == 0)
        {
        }
    }
    else
    {
        display.setTextSize(2);
        display.setCursor(0, 10);
        display.println("FACTORY");
        display.println("RESET");

        if (saveTemp && encoder->getValue())
        {
            systemProcess->factoryReset();
            encoder->setValue(0);
            saveTemp = false;
            initValue = false;
        }
        encoder->releasePosition();
    }
    display.display();
}

void OledDisplay::exit()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setFont(NULL);
    display.setCursor(40, 10);
    display.println("EXIT");
    display.drawBitmap(50, 30, logoExit, 24, 24, 1);
    display.display();
}

bool OledDisplay::selectedPush()
{
    if (encoder->singglePush())
    {

        selectedButton = !selectedButton;
        return selectedButton;
    }
    return selectedButton;
}