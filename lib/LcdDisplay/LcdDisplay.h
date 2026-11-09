#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <stdint.h>
#include <LiquidCrystal_I2C.h>

// Драйвер LCD 16x2 по I2C. Строка всегда дополняется пробелами до ширины
// дисплея, поэтому очистка экрана (и мерцание) не нужны.
class LcdDisplay
{
public:
    LcdDisplay(uint8_t address, uint8_t columns, uint8_t rows);

    void init();
    void printLine(uint8_t row, const char* text);

private:
    LiquidCrystal_I2C lcd;
    uint8_t columns;
};

#endif
