#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(uint8_t pin, uint16_t frequencyHz) : pin(pin), frequencyHz(frequencyHz) {}

void Buzzer::init()
{
    pinMode(pin, OUTPUT);
    off();
}

void Buzzer::on()
{
    tone(pin, frequencyHz);
}

void Buzzer::off()
{
    noTone(pin);
}
