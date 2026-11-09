#include "CommandParser.h"
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

namespace
{
struct Entry
{
    const char* name;
    CommandType type;
    bool needsValue;
};

const Entry COMMAND_TABLE[] = {
    {"help", CommandType::Help, false},
    {"status", CommandType::Status, false},
    {"sp", CommandType::SetPoint, true},
    {"hys", CommandType::Hysteresis, true},
    {"kp", CommandType::Kp, true},
    {"ki", CommandType::Ki, true},
    {"kd", CommandType::Kd, true},
    {"auto", CommandType::ModeAuto, false},
    {"man", CommandType::ModeManual, false},
    {"out", CommandType::Output, true},
    {"plot", CommandType::Plot, true},
};

constexpr size_t WORD_MAX_LENGTH = 8;

const char* skipSpaces(const char* text)
{
    while (*text != '\0' && isspace(static_cast<unsigned char>(*text)))
    {
        text++;
    }
    return text;
}
} // namespace

Command parseCommand(const char* line)
{
    Command result = {CommandType::None, 0.0f};

    const char* cursor = skipSpaces(line);
    if (*cursor == '\0')
    {
        return result;
    }

    char word[WORD_MAX_LENGTH + 1];
    size_t length = 0;
    while (*cursor != '\0' && !isspace(static_cast<unsigned char>(*cursor)))
    {
        if (length < WORD_MAX_LENGTH)
        {
            word[length++] = static_cast<char>(tolower(static_cast<unsigned char>(*cursor)));
        }
        cursor++;
    }
    word[length] = '\0';

    result.type = CommandType::Invalid;
    for (const Entry& entry : COMMAND_TABLE)
    {
        if (strcmp(word, entry.name) != 0)
        {
            continue;
        }

        cursor = skipSpaces(cursor);
        if (!entry.needsValue)
        {
            if (*cursor == '\0')
            {
                result.type = entry.type;
            }
            return result;
        }

        char* end = nullptr;
        const float value = static_cast<float>(strtod(cursor, &end));
        if (end != cursor && *skipSpaces(end) == '\0')
        {
            result.type = entry.type;
            result.value = value;
        }
        return result;
    }
    return result;
}
