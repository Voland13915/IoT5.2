#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

// Команды консоли, которые вводятся через STDIO (Serial Monitor, PuTTY, TeraTerm)
enum class CommandType
{
    None,       // пустая строка
    Invalid,    // неизвестная команда или неверный аргумент
    Help,       // help
    Status,     // status
    SetPoint,   // sp <°C>
    Hysteresis, // hys <°C>       (ЛР 5.1)
    Kp,         // kp <значение>  (ЛР 5.2)
    Ki,         // ki <значение>  (ЛР 5.2)
    Kd,         // kd <значение>  (ЛР 5.2)
    ModeAuto,   // auto           (ЛР 5.2)
    ModeManual, // man            (ЛР 5.2)
    Output,     // out <%>        (ЛР 5.2, ручной режим)
    Plot        // plot <0|1>     (0 = текст/CSV, 1 = Serial Plotter)
};

struct Command
{
    CommandType type;
    float value;
};

// Разбирает строку вида "sp 32.5" (регистр не важен)
Command parseCommand(const char* line);

#endif
