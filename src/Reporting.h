#ifndef REPORTING_H
#define REPORTING_H

#include <stdint.h>

// Отображение: LCD 16x2, вывод через STDIO (printf), Arduino Serial Plotter
void reportingInit();
void reportingPrintHelp();
void reportingPrintCsvHeader();
void reportingPrintStatus(uint32_t nowMs); // Serial Plotter или строка CSV
void reportingPrintTunings();              // команда status
void reportingRenderDisplay();             // две строки LCD

#endif
