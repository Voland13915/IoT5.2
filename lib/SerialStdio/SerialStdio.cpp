#include "SerialStdio.h"
#include <Arduino.h>

namespace
{
char lineBuffer[SERIAL_LINE_BUFFER_SIZE];
uint8_t lineLength = 0;
} // namespace

#if defined(ARDUINO_ARCH_AVR)
// AVR: в avr-libc stdout и stdin не привязаны к UART — создаём поток вручную
namespace
{
FILE serialStream;

int serialPutChar(char character, FILE* stream)
{
    (void)stream;
    if (character == '\n')
    {
        Serial.write('\r');
    }
    Serial.write(character);
    return 0;
}

int serialGetChar(FILE* stream)
{
    (void)stream;
    while (Serial.available() <= 0)
    {
    }
    return Serial.read();
}
} // namespace

void serialStdioInit(unsigned long baudRate)
{
    Serial.begin(baudRate);
    fdev_setup_stream(&serialStream, serialPutChar, serialGetChar, _FDEV_SETUP_RW);
    stdout = &serialStream;
    stdin = &serialStream;
}

const char* serialStdioPollLine()
{
    while (Serial.available() > 0)
    {
        const int character = getchar();
        if (character == '\r' || character == '\n')
        {
            if (lineLength == 0)
            {
                continue;
            }
            lineBuffer[lineLength] = '\0';
            lineLength = 0;
            return lineBuffer;
        }
        if (lineLength + 1 < SERIAL_LINE_BUFFER_SIZE)
        {
            lineBuffer[lineLength++] = static_cast<char>(character);
        }
    }
    return nullptr;
}
#else
void serialStdioInit(unsigned long baudRate)
{
    Serial.begin(baudRate);
}

const char* serialStdioPollLine()
{
    while (Serial.available() > 0)
    {
        const int character = Serial.read();
        if (character == '\r' || character == '\n')
        {
            if (lineLength == 0)
            {
                continue;
            }
            lineBuffer[lineLength] = '\0';
            lineLength = 0;
            return lineBuffer;
        }
        if (lineLength + 1 < SERIAL_LINE_BUFFER_SIZE)
        {
            lineBuffer[lineLength++] = static_cast<char>(character);
        }
    }
    return nullptr;
}
#endif
