#ifndef ALARM_INDICATOR_H
#define ALARM_INDICATOR_H

#include <stdint.h>
#include "Led.h"
#include "Buzzer.h"

// Световая и звуковая сигнализация: при тревоге мигает светодиод и звучит buzzer.
// update() вызывается из основного цикла, паузы внутри не используются.
class AlarmIndicator
{
public:
    AlarmIndicator(uint8_t ledPin, uint8_t buzzerPin, uint16_t frequencyHz, uint32_t blinkPeriodMs);

    void init();
    void update(bool alarmActive, uint32_t nowMs);

private:
    void setOutputs(bool isOn);

    Led led;
    Buzzer buzzer;
    uint32_t blinkPeriodMs;
    bool wasActive;
    bool phaseOn;
    uint32_t lastToggleMs;
};

#endif
