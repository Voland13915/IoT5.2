#ifndef BOUNDED_VALUE_H
#define BOUNDED_VALUE_H

#include <stdint.h>
#include "ControlMath.h"

// Число с границами и шагом: уставка, полуширина гистерезиса, ручной выход.
class BoundedValue
{
public:
    BoundedValue(float initial, float minimum, float maximum, float stepSize)
        : value(clampValue(initial, minimum, maximum)),
          minimum(minimum),
          maximum(maximum),
          stepSize(stepSize)
    {
    }

    float get() const
    {
        return value;
    }

    // true — значение действительно изменилось
    bool set(float newValue)
    {
        const float clamped = clampValue(newValue, minimum, maximum);
        const bool changed = (clamped != value);
        value = clamped;
        return changed;
    }

    // direction: +1 вверх, -1 вниз
    bool step(int8_t direction)
    {
        return set(value + direction * stepSize);
    }

private:
    float value;
    float minimum;
    float maximum;
    float stepSize;
};

#endif
