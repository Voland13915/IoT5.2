#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t address, uint8_t columns, uint8_t rows)
    : lcd(address, columns, rows), columns(columns)
{
}

void LcdDisplay::init()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void LcdDisplay::printLine(uint8_t row, const char* text)
{
    lcd.setCursor(0, row);

    uint8_t printed = 0;
    while (text[printed] != '\0' && printed < columns)
    {
        lcd.write(static_cast<uint8_t>(text[printed]));
        printed++;
    }
    while (printed < columns)
    {
        lcd.write(' ');
        printed++;
    }
}
