#include "TempSensor.h"
#include <math.h>

TempSensor::TempSensor(uint8_t pin, uint32_t minIntervalMs, uint8_t maxFailures)
    : dht(pin, DHT22),
      minIntervalMs(minIntervalMs),
      maxFailures(maxFailures),
      lastReadMs(0),
      hasRead(false),
      failureCount(0),
      lastTemperature(NAN),
      lastHumidity(NAN)
{
}

void TempSensor::init()
{
    dht.begin();
}

bool TempSensor::update(uint32_t nowMs)
{
    if (hasRead && (nowMs - lastReadMs) < minIntervalMs)
    {
        return false;
    }
    hasRead = true;
    lastReadMs = nowMs;

    const float temperature = dht.readTemperature();
    const float humidity = dht.readHumidity();
    if (isnan(temperature) || isnan(humidity))
    {
        if (failureCount < maxFailures)
        {
            failureCount++;
        }
        return false;
    }

    failureCount = 0;
    lastTemperature = temperature;
    lastHumidity = humidity;
    return true;
}

float TempSensor::getTemperature() const
{
    return lastTemperature;
}

float TempSensor::getHumidity() const
{
    return lastHumidity;
}

bool TempSensor::hasFault() const
{
    return failureCount >= maxFailures;
}
