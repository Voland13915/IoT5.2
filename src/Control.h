#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

void controlInit();

void controlPollButtons(uint32_t nowMs); // период BUTTON_POLL_PERIOD_MS: MODE, UP/DOWN
void controlHandleConsole();             // команды из консоли (STDIO)
void controlRunCycle(uint32_t nowMs);    // период CONTROL_PERIOD_MS: датчик -> PID -> ШИМ
void controlUpdateAlarm(uint32_t nowMs); // мигание LED и buzzer при тревоге

#endif
