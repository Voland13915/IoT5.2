#ifndef LED_H
#define LED_H

#include <stdint.h>

// Драйвер светодиода (анод через резистор к пину, катод к GND)
class Led
{
public:
    explicit Led(uint8_t pin);

    void init();
    void on();
    void off();
    void set(bool isOn);
    bool isOn() const;

private:
    uint8_t pin;
    bool state;
};

#endif
