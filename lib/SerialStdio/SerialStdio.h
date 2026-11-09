#ifndef SERIAL_STDIO_H
#define SERIAL_STDIO_H

#include <stdio.h>

// printf/snprintf с форматной строкой во Flash на AVR (экономия SRAM: у Uno всего 2 КБ)
#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#define PRINT_F(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#define SNPRINT_F(buffer, size, format, ...) snprintf_P(buffer, size, PSTR(format), ##__VA_ARGS__)
#else
#define PRINT_F(...) printf(__VA_ARGS__)
#define SNPRINT_F(buffer, size, ...) snprintf(buffer, size, __VA_ARGS__)
#endif

constexpr unsigned char SERIAL_LINE_BUFFER_SIZE = 24;

// Перенаправление stdout (printf) и stdin (getchar) в последовательный порт
void serialStdioInit(unsigned long baudRate);

// Неблокирующее чтение строки через getchar(): возвращает указатель на строку,
// когда получен символ конца строки, иначе nullptr
const char* serialStdioPollLine();

#endif
