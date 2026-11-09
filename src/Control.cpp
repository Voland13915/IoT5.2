#include "Control.h"
#include <Arduino.h>
#include <math.h>
#include "AppConfig.h"
#include "AlarmIndicator.h"
#include "AlarmMonitor.h"
#include "Button.h"
#include "CommandParser.h"
#include "Led.h"
#include "OverTempGuard.h"
#include "PidController.h"
#include "PwmOutput.h"
#include "Reporting.h"
#include "SerialStdio.h"
#include "Signals.h"
#include "TempSensor.h"
#ifdef SIMULATE_PLANT
#include "ThermalModel.h"
#endif

namespace
{
TempSensor sensor(DHT_PIN, SENSOR_MIN_INTERVAL_MS, SENSOR_MAX_FAILURES);
Button upButton(BUTTON_UP_PIN, BUTTON_DEBOUNCE_SAMPLES, BUTTON_REPEAT_DELAY_SAMPLES, BUTTON_REPEAT_PERIOD_SAMPLES);
Button downButton(BUTTON_DOWN_PIN, BUTTON_DEBOUNCE_SAMPLES, BUTTON_REPEAT_DELAY_SAMPLES, BUTTON_REPEAT_PERIOD_SAMPLES);
Button modeButton(BUTTON_MODE_PIN, BUTTON_DEBOUNCE_SAMPLES); // без автоповтора
PwmOutput heater(HEATER_PWM_PIN);
Led statusLed(STATUS_LED_PIN);
AlarmIndicator alarmIndicator(ALARM_LED_PIN, BUZZER_PIN, BUZZER_FREQUENCY_HZ, ALARM_BLINK_PERIOD_MS);
PidController pid(PID_KP_DEFAULT, PID_KI_DEFAULT, PID_KD_DEFAULT, CONTROL_PERIOD_S, OUTPUT_MIN_PERCENT,
                  OUTPUT_MAX_PERCENT);
AlarmMonitor alarmMonitor(ALARM_ERROR_C, ALARM_ARM_BAND_C, ALARM_DELAY_MS);
OverTempGuard overTempGuard(OVER_TEMP_LIMIT_C, OVER_TEMP_RELEASE_C);
#ifdef SIMULATE_PLANT
ThermalModel plant(AMBIENT_TEMP_C, PLANT_MAX_TEMP_C, PLANT_TIME_CONSTANT_S, CONTROL_PERIOD_S, PLANT_DELAY_SAMPLES);
#endif

// До первого отсчёта температура неизвестна (NAN): чтобы не испортить PID, берём уставку
float measuredOrSetPoint()
{
    return isnan(temperature) ? setPoint.get() : temperature;
}

void applyOutput(float percent)
{
    outputPercent = percent;
    heater.setPercent(percent);
    statusLed.set(percent > OUTPUT_MIN_PERCENT);
}

// Возвращает true, если получен новый отсчёт температуры
bool readTemperature(uint32_t nowMs)
{
#ifdef SIMULATE_PLANT
    (void)nowMs;
    temperature = plant.step(outputPercent);
    humidity = SIMULATED_HUMIDITY_PERCENT;
    sensorFault = false;
    return true;
#else
    const bool isFresh = sensor.update(nowMs);
    sensorFault = sensor.hasFault();
    if (isFresh)
    {
        temperature = sensor.getTemperature();
        humidity = sensor.getHumidity();
    }
    return isFresh;
#endif
}

void setMode(bool isAuto)
{
    if (autoMode == isAuto)
    {
        return;
    }
    autoMode = isAuto;
    alarmMonitor.notifySetPointChanged();

    if (autoMode)
    {
        // Безударный переход MAN -> AUTO: PID стартует с текущего выхода
        pid.reset(outputPercent, setPoint.get(), measuredOrSetPoint());
    }
    else
    {
        manualOutput.set(outputPercent);
    }
    PRINT_F("OK: mode=%s\n", autoMode ? "AUTO" : "MAN");
}

void setTunings(float kp, float ki, float kd)
{
    pid.setTunings(kp, ki, kd);
    pidKp = pid.getKp();
    pidKi = pid.getKi();
    pidKd = pid.getKd();
}
} // namespace

void controlInit()
{
    sensor.init();
    upButton.init();
    downButton.init();
    modeButton.init();
    heater.init();
    statusLed.init();
    alarmIndicator.init();
    if (!plotMode)
    {
        reportingPrintCsvHeader();
    }
}

void controlPollButtons(uint32_t nowMs)
{
    (void)nowMs;
    if (modeButton.update())
    {
        setMode(!autoMode);
    }

    int8_t direction = 0;
    if (upButton.update())
    {
        direction = 1;
    }
    if (downButton.update())
    {
        direction = -1;
    }
    if (direction == 0)
    {
        return;
    }

    if (autoMode)
    {
        if (setPoint.step(direction))
        {
            alarmMonitor.notifySetPointChanged();
        }
    }
    else
    {
        manualOutput.step(direction);
    }
}

void controlHandleConsole()
{
    const char* line = serialStdioPollLine();
    if (line == nullptr)
    {
        return;
    }

    const Command command = parseCommand(line);
    switch (command.type)
    {
    case CommandType::None:
        break;
    case CommandType::Help:
        reportingPrintHelp();
        break;
    case CommandType::Status:
        reportingPrintTunings();
        break;
    case CommandType::SetPoint:
        setPoint.set(command.value);
        alarmMonitor.notifySetPointChanged();
        PRINT_F("OK: SP=%.1fC\n", setPoint.get());
        break;
    case CommandType::Kp:
        setTunings(command.value, pid.getKi(), pid.getKd());
        PRINT_F("OK: Kp=%.2f\n", pidKp);
        break;
    case CommandType::Ki:
        setTunings(pid.getKp(), command.value, pid.getKd());
        PRINT_F("OK: Ki=%.3f\n", pidKi);
        break;
    case CommandType::Kd:
        setTunings(pid.getKp(), pid.getKi(), command.value);
        PRINT_F("OK: Kd=%.2f\n", pidKd);
        break;
    case CommandType::ModeAuto:
        setMode(true);
        break;
    case CommandType::ModeManual:
        setMode(false);
        break;
    case CommandType::Output:
        manualOutput.set(command.value);
        PRINT_F("OK: manual OUT=%.0f%%\n", manualOutput.get());
        break;
    case CommandType::Plot:
        plotMode = (command.value != 0.0f);
        if (!plotMode)
        {
            reportingPrintCsvHeader();
        }
        break;
    default:
        PRINT_F("ERR: unknown command, type help\n");
        break;
    }
}

void controlRunCycle(uint32_t nowMs)
{
    const bool isFresh = readTemperature(nowMs);

    if (sensorFault)
    {
        applyOutput(OUTPUT_MIN_PERCENT);
        pid.reset(OUTPUT_MIN_PERCENT, setPoint.get(), measuredOrSetPoint());
        PRINT_F("ERR: sensor fault, output 0\n");
        return;
    }
    if (!isFresh)
    {
        return;
    }

    float output;
    overTempTripped = overTempGuard.update(temperature);
    if (overTempTripped)
    {
        output = OUTPUT_MIN_PERCENT; // защита от перегрева важнее регулятора
        pid.reset(output, setPoint.get(), measuredOrSetPoint());
    }
    else if (autoMode)
    {
        output = pid.compute(setPoint.get(), temperature);
        alarmActive = alarmMonitor.update(setPoint.get() - temperature, nowMs);
    }
    else
    {
        output = manualOutput.get();
        pid.reset(output, setPoint.get(), measuredOrSetPoint()); // PID "следит" за ручным выходом
        alarmActive = false;
    }
    applyOutput(output);

    reportingPrintStatus(nowMs);
}

void controlUpdateAlarm(uint32_t nowMs)
{
    alarmIndicator.update(alarmActive || sensorFault, nowMs);
}
