#include "AlarmIndicator.h"

AlarmIndicator::AlarmIndicator(uint8_t ledPin, uint8_t buzzerPin, uint16_t frequencyHz, uint32_t blinkPeriodMs)
    : led(ledPin),
      buzzer(buzzerPin, frequencyHz),
      blinkPeriodMs(blinkPeriodMs),
      wasActive(false),
      phaseOn(false),
      lastToggleMs(0)
{
}

void AlarmIndicator::init()
{
    led.init();
    buzzer.init();
}

void AlarmIndicator::setOutputs(bool isOn)
{
    led.set(isOn);
    if (isOn)
    {
        buzzer.on();
    }
    else
    {
        buzzer.off();
    }
}

void AlarmIndicator::update(bool alarmActive, uint32_t nowMs)
{
    if (!alarmActive)
    {
        if (wasActive)
        {
            wasActive = false;
            phaseOn = false;
            setOutputs(false);
        }
        return;
    }

    if (!wasActive)
    {
        wasActive = true;
        phaseOn = true;
        lastToggleMs = nowMs;
        setOutputs(true);
        return;
    }

    if ((nowMs - lastToggleMs) >= blinkPeriodMs)
    {
        lastToggleMs = nowMs;
        phaseOn = !phaseOn;
        setOutputs(phaseOn);
    }
}
