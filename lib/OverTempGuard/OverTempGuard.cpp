#include "OverTempGuard.h"

OverTempGuard::OverTempGuard(float limitC, float releaseC)
    : limitC(limitC), releaseC(releaseC), tripped(false)
{
}

bool OverTempGuard::update(float temperatureC)
{
    if (temperatureC >= limitC)
    {
        tripped = true;
    }
    else if (temperatureC <= releaseC)
    {
        tripped = false;
    }
    return tripped;
}

bool OverTempGuard::isTripped() const
{
    return tripped;
}
