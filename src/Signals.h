#ifndef SIGNALS_H
#define SIGNALS_H

#include "BoundedValue.h"

// Общее состояние процесса. Control (provider) пишет, Reporting (consumer) читает.
// Прерываний нет, поэтому volatile не нужен.

// Задание
extern BoundedValue setPoint;       // уставка, °C (режим AUTO)
extern BoundedValue manualOutput;   // ручной выход, % (режим MAN)

// Измерения
extern float temperature;           // °C, NAN до первого отсчёта
extern float humidity;              // %

// Состояние регулятора
extern float outputPercent;         // выход регулятора, 0..100 %
extern float pidKp;                 // текущие коэффициенты (для команды status)
extern float pidKi;
extern float pidKd;
extern bool autoMode;               // true = AUTO (PID), false = MAN
extern bool overTempTripped;
extern bool alarmActive;
extern bool sensorFault;

// Режим вывода: true — формат Serial Plotter, false — CSV
extern bool plotMode;

#endif
