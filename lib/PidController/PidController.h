#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

// Дискретный PID-регулятор: производная по измерению, антивиндап ограничением
// интеграла, безударный сброс. Не зависит от оборудования, поэтому
// проверяется модульными тестами на ПК.
class PidController
{
public:
    PidController(float kp, float ki, float kd, float samplePeriodS, float outputMin, float outputMax);

    void setTunings(float newKp, float newKi, float newKd);
    float getKp() const;
    float getKi() const;
    float getKd() const;

    // Один шаг регулирования, возвращает выход в диапазоне [outputMin, outputMax]
    float compute(float setPoint, float measured);

    // Безударное переключение: подгоняет интеграл так, чтобы выход остался прежним
    void reset(float currentOutput, float setPoint, float measured);

    float getLastOutput() const;
    float getIntegral() const;

private:
    float kp;
    float ki;
    float kd;
    float samplePeriodS;
    float outputMin;
    float outputMax;
    float integral;
    float previousMeasured;
    bool hasPrevious;
    float lastOutput;
};

#endif
