#include "PwmOutput.h"
#include <Arduino.h>
#include "ControlMath.h"

constexpr uint8_t PwmOutput::PWM_MAX_VALUE;
constexpr float PwmOutput::PERCENT_MAX;

PwmOutput::PwmOutput(uint8_t pin) : pin(pin), percent(0.0f) {}

void PwmOutput::init()
{
    pinMode(pin, OUTPUT);
    setPercent(0.0f);
}

void PwmOutput::setPercent(float newPercent)
{
    percent = clampValue(newPercent, 0.0f, PERCENT_MAX);
    analogWrite(pin, static_cast<int>(percent * PWM_MAX_VALUE / PERCENT_MAX + 0.5f));
}

float PwmOutput::getPercent() const
{
    return percent;
}
