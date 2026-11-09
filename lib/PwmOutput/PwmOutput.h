#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H

#include <stdint.h>

// Аналоговое управление мощностью нагрузки через ШИМ (analogWrite): 0..100 % -> 0..255.
// Uno: подходят выводы 3, 5, 6, 9, 10, 11. Вывод 9 (Timer1, около 490 Гц) выбран потому,
// что tone() для buzzer использует Timer2 и нарушил бы ШИМ на выводах 3 и 11.
class PwmOutput
{
public:
    explicit PwmOutput(uint8_t pin);

    void init();
    void setPercent(float percent);
    float getPercent() const;

    static constexpr uint8_t PWM_MAX_VALUE = 255;
    static constexpr float PERCENT_MAX = 100.0f;

private:
    uint8_t pin;
    float percent;
};

#endif
