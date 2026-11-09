#include "PidController.h"
#include "ControlMath.h"

PidController::PidController(float kp, float ki, float kd, float samplePeriodS, float outputMin, float outputMax)
    : kp(kp),
      ki(ki),
      kd(kd),
      samplePeriodS(samplePeriodS),
      outputMin(outputMin),
      outputMax(outputMax),
      integral(0.0f),
      previousMeasured(0.0f),
      hasPrevious(false),
      lastOutput(outputMin)
{
}

void PidController::setTunings(float newKp, float newKi, float newKd)
{
    kp = newKp;
    ki = newKi;
    kd = newKd;
}

float PidController::getKp() const
{
    return kp;
}

float PidController::getKi() const
{
    return ki;
}

float PidController::getKd() const
{
    return kd;
}

float PidController::getLastOutput() const
{
    return lastOutput;
}

float PidController::getIntegral() const
{
    return integral;
}

float PidController::compute(float setPoint, float measured)
{
    const float error = setPoint - measured;
    const float proportional = kp * error;
    // Производная по измерению: скачок уставки не даёт выброса выхода
    const float derivative = hasPrevious ? -kd * (measured - previousMeasured) / samplePeriodS : 0.0f;

    // Антивиндап: интеграл не копится, если выход уже в насыщении
    // и ошибка толкает его дальше за границу
    const float integralCandidate = integral + ki * samplePeriodS * error;
    const float unsaturated = proportional + integralCandidate + derivative;
    const bool isPushingHigh = unsaturated > outputMax && error > 0.0f;
    const bool isPushingLow = unsaturated < outputMin && error < 0.0f;
    if (!isPushingHigh && !isPushingLow)
    {
        integral = clampValue(integralCandidate, outputMin, outputMax);
    }

    lastOutput = clampValue(proportional + integral + derivative, outputMin, outputMax);
    previousMeasured = measured;
    hasPrevious = true;
    return lastOutput;
}

void PidController::reset(float currentOutput, float setPoint, float measured)
{
    const float clamped = clampValue(currentOutput, outputMin, outputMax);
    integral = clampValue(clamped - kp * (setPoint - measured), outputMin, outputMax);
    previousMeasured = measured;
    hasPrevious = true;
    lastOutput = clamped;
}
