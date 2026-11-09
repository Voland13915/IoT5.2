#include "Button.h"
#include <Arduino.h>

Button::Button(uint8_t pin, uint8_t debounceSamples, uint8_t repeatDelaySamples, uint8_t repeatPeriodSamples)
    : pin(pin),
      debounceSamples(debounceSamples),
      repeatDelaySamples(repeatDelaySamples),
      repeatPeriodSamples(repeatPeriodSamples),
      sameSampleCount(0),
      heldSamples(0),
      repeatCounter(0),
      lastRawPressed(false),
      stablePressed(false)
{
}

void Button::init()
{
    pinMode(pin, INPUT_PULLUP);
}

bool Button::update()
{
    const bool rawPressed = (digitalRead(pin) == LOW);

    if (rawPressed == lastRawPressed)
    {
        if (sameSampleCount < debounceSamples)
        {
            sameSampleCount++;
        }
    }
    else
    {
        lastRawPressed = rawPressed;
        sameSampleCount = 1;
    }

    const bool isStable = (sameSampleCount >= debounceSamples);
    if (isStable && rawPressed != stablePressed)
    {
        stablePressed = rawPressed;
        heldSamples = 0;
        repeatCounter = 0;
        return stablePressed; // событие при переходе в "нажата"
    }

    if (stablePressed && repeatPeriodSamples > 0)
    {
        if (heldSamples < repeatDelaySamples)
        {
            heldSamples++;
        }
        else
        {
            repeatCounter++;
            if (repeatCounter >= repeatPeriodSamples)
            {
                repeatCounter = 0;
                return true; // тик автоповтора
            }
        }
    }
    return false;
}

bool Button::isPressed() const
{
    return stablePressed;
}
