#ifndef THERMAL_MODEL_H
#define THERMAL_MODEL_H

#include <stdint.h>

// Модель объекта «нагреватель + воздух»: апериодическое звено 1-го порядка
// с транспортной задержкой. Используется в сборке SIMULATE_PLANT вместо реального
// датчика (в эмуляторе температура DHT22 не зависит от нагревателя) и в тестах на ПК.
class ThermalModel
{
public:
    static constexpr uint8_t MAX_DELAY_SAMPLES = 8;

    ThermalModel(float ambientC, float maxTempC, float timeConstantS, float samplePeriodS, uint8_t delaySamples);

    // powerPercent: 0..100. Возвращает новую температуру.
    float step(float powerPercent);
    float getTemperature() const;
    void setAmbient(float newAmbientC);
    void reset(float temperatureC);

private:
    float ambientC;
    float maxTempC;
    float timeConstantS;
    float samplePeriodS;
    uint8_t delaySamples;
    uint8_t head;
    float delayLine[MAX_DELAY_SAMPLES];
    float temperature;
};

#endif
