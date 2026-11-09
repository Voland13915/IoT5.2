#include "Reporting.h"
#include <stdio.h>
#include "AppConfig.h"
#include "LcdDisplay.h"
#include "SerialStdio.h"
#include "Signals.h"

namespace
{
LcdDisplay lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
} // namespace

void reportingInit()
{
    lcd.init();
    lcd.printLine(0, "Lab 5.2 PID");
    lcd.printLine(1, "Heater control");
}

void reportingPrintHelp()
{
    PRINT_F("Commands: help | status | sp <C> | kp|ki|kd <v> | auto | man | out <%%> | plot <0|1>\n");
}

void reportingPrintCsvHeader()
{
    PRINT_F("t_s,SP,PV,OUT,MODE,RH\n");
}

void reportingPrintStatus(uint32_t nowMs)
{
    if (plotMode)
    {
        // Формат "имя:значение" для Arduino Serial Plotter
        PRINT_F("SetPoint:%.1f,Value:%.1f,Output:%.1f\n", setPoint.get(), temperature, outputPercent);
    }
    else
    {
        // Строка CSV: удобно сохранять лог терминала и открывать в Excel
        PRINT_F("%lu,%.1f,%.1f,%.1f,%s,%.0f\n", nowMs / 1000UL, setPoint.get(), temperature,
                outputPercent, autoMode ? "AUTO" : "MAN", humidity);
    }
}

void reportingPrintTunings()
{
    PRINT_F("SP=%.1f PV=%.1f OUT=%.1f%% MODE=%s Kp=%.2f Ki=%.3f Kd=%.2f\n", setPoint.get(),
            temperature, outputPercent, autoMode ? "AUTO" : "MAN", pidKp, pidKi, pidKd);
}

void reportingRenderDisplay()
{
    char line[LCD_COLUMNS + 1];

    if (sensorFault)
    {
        lcd.printLine(0, "SENSOR ERROR");
        lcd.printLine(1, "Output: 0%");
        return;
    }

    SNPRINT_F(line, sizeof(line), "SP:%4.1f PV:%4.1f", setPoint.get(), temperature);
    lcd.printLine(0, line);
    SNPRINT_F(line, sizeof(line), "OUT:%3d%% %-4s %c", static_cast<int>(outputPercent + 0.5f),
              autoMode ? "AUTO" : "MAN", alarmActive ? '!' : ' ');
    lcd.printLine(1, line);
}
