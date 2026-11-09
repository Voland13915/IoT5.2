#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

// Драйвер пассивного buzzer: тон заданной частоты через tone() (Timer2 на Uno)
class Buzzer
{
public:
    Buzzer(uint8_t pin, uint16_t frequencyHz);

    void init();
    void on();
    void off();

private:
    uint8_t pin;
    uint16_t frequencyHz;
};

#endif
