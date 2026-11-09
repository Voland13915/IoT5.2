#ifndef ALARM_MONITOR_H
#define ALARM_MONITOR_H

#include <stdint.h>

// Тревога «потеря регулирования»: |ошибка| > errorLimit дольше delayMs.
// Включается только после того, как система вошла в зону armBand
// (иначе тревога срабатывала бы при каждом разогреве или смене уставки).
class AlarmMonitor
{
public:
    AlarmMonitor(float errorLimit, float armBand, uint32_t delayMs);

    void notifySetPointChanged();
    bool update(float error, uint32_t nowMs);
    bool isActive() const;
    bool isArmed() const;

private:
    float errorLimit;
    float armBand;
    uint32_t delayMs;
    bool armed;
    bool exceeding;
    bool active;
    uint32_t exceedStartMs;
};

#endif
