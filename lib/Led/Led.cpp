#include "Led.h"
#include <Arduino.h>

Led::Led(uint8_t pin) : pin(pin), state(false) {}

void Led::init()
{
    pinMode(pin, OUTPUT);
    off();
}

void Led::on()
{
    state = true;
    digitalWrite(pin, HIGH);
}

void Led::off()
{
    state = false;
    digitalWrite(pin, LOW);
}

void Led::set(bool isOn)
{
    if (isOn)
    {
        on();
    }
    else
    {
        off();
    }
}

bool Led::isOn() const
{
    return state;
}
