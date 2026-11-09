#include <Arduino.h>
#include <stdio.h>
#include "AppConfig.h"
#include "Control.h"
#include "Reporting.h"
#include "SerialStdio.h"

namespace
{
uint32_t lastButtonPollMs = 0;
uint32_t lastControlMs = 0;
uint32_t lastDisplayMs = 0;
} // namespace

void setup()
{
    serialStdioInit(SERIAL_BAUD_RATE);
    reportingInit();
    controlInit();
}

// Основной цикл без delay(): кнопки, консоль, PID и дисплей
// работают со своими периодами по millis().
void loop()
{
    const uint32_t nowMs = millis();

    if (nowMs - lastButtonPollMs >= BUTTON_POLL_PERIOD_MS)
    {
        lastButtonPollMs = nowMs;
        controlPollButtons(nowMs);
    }

    controlHandleConsole();

    if (nowMs - lastControlMs >= CONTROL_PERIOD_MS)
    {
        lastControlMs = nowMs;
        controlRunCycle(nowMs);
    }

    if (nowMs - lastDisplayMs >= DISPLAY_PERIOD_MS)
    {
        lastDisplayMs = nowMs;
        reportingRenderDisplay();
    }

    controlUpdateAlarm(nowMs);
}
