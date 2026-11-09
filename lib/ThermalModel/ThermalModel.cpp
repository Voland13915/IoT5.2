#include "ThermalModel.h"

constexpr uint8_t ThermalModel::MAX_DELAY_SAMPLES;

ThermalModel::ThermalModel(float ambientC, float maxTempC, float timeConstantS, float samplePeriodS, uint8_t delaySamples)
    : ambientC(ambientC),
      maxTempC(maxTempC),
      timeConstantS(timeConstantS),
      samplePeriodS(samplePeriodS),
      delaySamples(delaySamples > MAX_DELAY_SAMPLES ? MAX_DELAY_SAMPLES : delaySamples),
      head(0),
      temperature(ambientC)
{
    for (uint8_t i = 0; i < MAX_DELAY_SAMPLES; i++)
    {
        delayLine[i] = 0.0f;
    }
}

float ThermalModel::getTemperature() const
{
    return temperature;
}

void ThermalModel::setAmbient(float newAmbientC)
{
    ambientC = newAmbientC;
}

void ThermalModel::reset(float temperatureC)
{
    temperature = temperatureC;
    head = 0;
    for (uint8_t i = 0; i < MAX_DELAY_SAMPLES; i++)
    {
        delayLine[i] = 0.0f;
    }
}

float ThermalModel::step(float powerPercent)
{
    float appliedPercent = powerPercent;
    if (delaySamples > 0)
    {
        appliedPercent = delayLine[head];
        delayLine[head] = powerPercent;
        head = (head + 1) % delaySamples;
    }

    const float targetTemp = ambientC + (maxTempC - ambientC) * appliedPercent / 100.0f;
    temperature += (samplePeriodS / timeConstantS) * (targetTemp - temperature);
    return temperature;
}
