#include "charging_algorithm.h"

ChargingAlgorithm::ChargingAlgorithm(uint8_t buck_en, uint8_t buck_in)
{
    buckEN = buck_en;
    buckIN = buck_in;
};

void ChargingAlgorithm::setup(SystemProcess *system_process, SensorReading *sensors, DeviceProtection *device_protection)
{
    systemProcess = system_process;
    sensor = sensors;
    deviceProtection = device_protection;

    pinMode(buckEN, OUTPUT);
    ledcSetup(PWM_CHANEL, PWM_FREQUENCY, PWM_RESOLUTION); // Set PWM Parameters
    ledcAttachPin(buckIN, PWM_CHANEL);                    // Set pin as PWM
    ledcWrite(PWM_CHANEL, PWM);                           // Write PWM value at startup (duty = 0)
    pwmMax = pow(2, PWM_RESOLUTION) - 1;                  // Get PWM Max Bit Ceiling
    pwmMaxLimited = (PWM_MAX_DC * pwmMax) / 100.000;      // Get maximum PWM Duty Cycle (pwm limiting protection)
}

void ChargingAlgorithm::buckEnable()
{
    buckState = true;
    digitalWrite(buckEN, HIGH);
}

void ChargingAlgorithm::buckDisable()
{
    digitalWrite(buckEN, LOW);
    PWM = 0;
}

void ChargingAlgorithm::predictedPWM(float voltage_input, float voltage_batt)
{
    if (voltage_input <= 0) // Prevents Indefinite Answer when voltageInput is zero
    {
        PPWM = 0;
    }
    else
    { // Compute for predictive PWM Floor and voltage_output in variable
        PPWM = (PPWMargin * pwmMax * voltage_batt) / (100.00 * voltage_input);
    }
    PPWM = constrain(PPWM, 0, pwmMaxLimited);
}

void ChargingAlgorithm ::modulationPWM(bool output_mode, float voltage_input, float voltage_batt)
{
    if (output_mode == 0) // PSU MODE PWM = PWM OVERFLOW PROTECTION (limit floor to 0% and ceiling to maximim allowable duty cycle)
    {
        PWM = constrain(PWM, 0, pwmMaxLimited);
    }
    else
    {
        predictedPWM(voltage_input, voltage_batt); // Runs and computes for predictive pwm floor
        PWM = constrain(PWM, PPWM, pwmMaxLimited); // CHARGER MODE PWM - limit floor to PPWM and ceiling to maximim allowable duty cycle)
    }
    ledcWrite(PWM_CHANEL, PWM); // Set PWM duty cycle and write to GPIO when buck is enabled
    buckEnable();               // Turn on MPPT buck (IR2104)
}

void ChargingAlgorithm::chargingAlgorithm()
{
    loopTimeStart = micros();                                     // Record Start Time
    chargingLoopTimme = (loopTimeStart - loopTimeEnd) / 1000.000; // Compute Loop Cycle Speed (mS)
    loopTimeEnd = micros();                                       // Record End Time

    if (deviceProtection->getERR() > 0 || chargingPause == 1)
    {
        buckDisable(); // ERROR PRESENT  - Turn off MPPT buck when error is present or when chargingPause is used for a pause override
    }
    else
    {
        if (deviceProtection->getREC() == 1)
        {                                                    // IUV RECOVERY - (Only active for charging mode)
            deviceProtection->setREC(0);                     // Reset IUV recovery boolean identifier
            Serial.println("> Solar Panel Detected");        // Display serial message
            Serial.print("> Computing For Predictive PWM "); // Display serial message
            for (int i = 0; i < 40; i++)
            {
                Serial.print(".");
                delay(30);
            }                   // For loop "loading... effect
            Serial.println(""); // Display a line break on serial for next lines
            // Read_Sensors();
            predictedPWM(sensor->getVoltageInput(), sensor->getVoltageBattery());
            PWM = PPWM;
        }
        else
        { // NO ERROR PRESENT  - Continue power conversion
            /////////////////////// CC-CV BUCK PSU ALGORITHM //////////////////////////////
            if (!systemProcess->getMPPTMode())
            { // CC-CV PSU MODE
                if (sensor->getCurrentBattery() > systemProcess->getCurrentCharging())
                {
                    PWM--;
                } // Current Is Above → Decrease Duty Cycle
                else if (sensor->getVoltageBattery() > systemProcess->getVoltageBatteryMax())
                {
                    PWM--;
                } // Voltage Is Above → Decrease Duty Cycle
                else if (sensor->getVoltageBattery() < systemProcess->getVoltageBatteryMax())
                {
                    PWM++;
                } // Increase duty cycle when output is below charging voltage (for CC-CV only mode)
                else
                {
                }                                                                                                      // Do nothing when set output voltage is reached
                modulationPWM(systemProcess->getOutputMode(), sensor->getVoltageInput(), sensor->getVoltageBattery()); // Set PWM signal to Buck PWM GPIO
            }
            /////////////////////// MPPT & CC-CV CHARGING ALGORITHM ///////////////////////
            else
            {
                if (sensor->getCurrentBattery() > systemProcess->getCurrentCharging())
                {
                    PWM--;
                } // Current Is Above → Decrease Duty Cycle
                else if (sensor->getVoltageBattery() > systemProcess->getVoltageBatteryMax())
                {
                    PWM--;
                } // Voltage Is Above → Decrease Duty Cycle
                else
                {
                    if (sensor->getPowerInput() > powerInputPrev && sensor->getVoltageInput() > voltageInputPrev) // MPPT ALGORITHM
                    {
                        PWM--; //  ↑P ↑V ; →MPP  //D--
                    }
                    else if (sensor->getPowerInput() > powerInputPrev && sensor->getVoltageInput() < voltageInputPrev)
                    {
                        PWM++; //  ↑P ↓V ; MPP←  //D++
                    }
                    else if (sensor->getPowerInput() < powerInputPrev && sensor->getVoltageInput() > voltageInputPrev)
                    {
                        PWM++; //  ↓P ↑V ; MPP→  //D++
                    }
                    else if (sensor->getPowerInput() < powerInputPrev && sensor->getVoltageInput() < voltageInputPrev)
                    {
                        PWM--; //  ↓P ↓V ; ←MPP  //D--
                    }
                    else if (sensor->getVoltageBattery() < systemProcess->getVoltageBatteryMax())
                    {
                        PWM++; //  MP MV ; MPP Reached -
                    }
                    powerInputPrev = sensor->getPowerInput();     // Store Previous Recorded Power
                    voltageInputPrev = sensor->getVoltageInput(); // Store Previous Recorded Voltage
                }
                modulationPWM(systemProcess->getOutputMode(), sensor->getVoltageInput(), sensor->getVoltageBattery()); // Set PWM signal to Buck PWM GPIO
            }
        }
    }
}