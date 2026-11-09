#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <stdint.h>
#include <DHT.h>

// Драйвер DHT22: соблюдает минимальный интервал опроса и считает ошибки чтения.
// Датчик допускает опрос не чаще одного раза в 2 с.
class TempSensor
{
public:
    TempSensor(uint8_t pin, uint32_t minIntervalMs, uint8_t maxFailures);

    void init();
    bool update(uint32_t nowMs); // true — получен новый корректный отсчёт

    float getTemperature() const;
    float getHumidity() const;
    bool hasFault() const; // maxFailures неудачных чтений подряд

private:
    DHT dht;
    uint32_t minIntervalMs;
    uint8_t maxFailures;
    uint32_t lastReadMs;
    bool hasRead;
    uint8_t failureCount;
    float lastTemperature;
    float lastHumidity;
};

#endif
