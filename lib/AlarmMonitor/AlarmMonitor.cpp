#include "AlarmMonitor.h"
#include <math.h>

AlarmMonitor::AlarmMonitor(float errorLimit, float armBand, uint32_t delayMs)
    : errorLimit(errorLimit),
      armBand(armBand),
      delayMs(delayMs),
      armed(false),
      exceeding(false),
      active(false),
      exceedStartMs(0)
{
}

void AlarmMonitor::notifySetPointChanged()
{
    armed = false;
    exceeding = false;
    active = false;
}

bool AlarmMonitor::update(float error, uint32_t nowMs)
{
    const float magnitude = fabsf(error);

    if (!armed)
    {
        if (magnitude > armBand)
        {
            return false;
        }
        armed = true;
    }

    if (magnitude > errorLimit)
    {
        if (!exceeding)
        {
            exceeding = true;
            exceedStartMs = nowMs;
        }
        if (nowMs - exceedStartMs >= delayMs)
        {
            active = true;
        }
    }
    else
    {
        exceeding = false;
        active = false;
    }
    return active;
}

bool AlarmMonitor::isActive() const
{
    return active;
}

bool AlarmMonitor::isArmed() const
{
    return armed;
}
