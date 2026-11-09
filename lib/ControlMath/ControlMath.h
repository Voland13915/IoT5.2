#ifndef CONTROL_MATH_H
#define CONTROL_MATH_H

// Ограничение значения диапазоном [low, high]
inline float clampValue(float value, float low, float high)
{
    if (value < low)
    {
        return low;
    }
    if (value > high)
    {
        return high;
    }
    return value;
}

#endif
