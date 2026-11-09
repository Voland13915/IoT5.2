#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

// Драйвер кнопки: подключение к GND, внутренний подтягивающий резистор
// (нажата = LOW). Антидребезг по числу одинаковых опросов подряд,
// поэтому update() нужно вызывать с постоянным периодом.
// Автоповтор: если кнопка удерживается repeatDelaySamples опросов, update()
// возвращает true каждые repeatPeriodSamples опросов (0 — автоповтор выключен).
class Button
{
public:
    Button(uint8_t pin, uint8_t debounceSamples, uint8_t repeatDelaySamples = 0, uint8_t repeatPeriodSamples = 0);

    void init();
    bool update();          // true — в момент нажатия (фронт) и по тикам автоповтора
    bool isPressed() const; // устойчивое (отфильтрованное) состояние

private:
    uint8_t pin;
    uint8_t debounceSamples;
    uint8_t repeatDelaySamples;
    uint8_t repeatPeriodSamples;
    uint8_t sameSampleCount;
    uint8_t heldSamples;
    uint8_t repeatCounter;
    bool lastRawPressed;
    bool stablePressed;
};

#endif
