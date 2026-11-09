// Модульные тесты логики ЛР 5.2 (без оборудования): pio test -e native
#include <unity.h>
#include <math.h>
#include "AlarmMonitor.h"
#include "BoundedValue.h"
#include "CommandParser.h"
#include "OverTempGuard.h"
#include "PidController.h"
#include "ThermalModel.h"

// Параметры такие же, как в include/AppConfig.h
constexpr float AMBIENT_C = 22.0f;
constexpr float PLANT_MAX_C = 70.0f;
constexpr float PLANT_TAU_S = 180.0f;
constexpr float SAMPLE_S = 2.0f;
constexpr uint8_t PLANT_DELAY = 2;

void setUp(void) {}
void tearDown(void) {}

// ---------- BoundedValue ----------
void test_bounded_value_clamps_and_steps()
{
    BoundedValue v(30.0f, 15.0f, 45.0f, 0.5f);
    TEST_ASSERT_TRUE(v.step(+1));
    TEST_ASSERT_EQUAL_FLOAT(30.5f, v.get());
    TEST_ASSERT_TRUE(v.set(100.0f));
    TEST_ASSERT_EQUAL_FLOAT(45.0f, v.get());
    TEST_ASSERT_FALSE(v.step(+1)); // уже на верхней границе
}

// ---------- PID ----------
void test_pid_proportional_only()
{
    PidController pid(2.0f, 0.0f, 0.0f, 2.0f, 0.0f, 100.0f);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, pid.compute(30.0f, 20.0f));   // 2 * 10
    TEST_ASSERT_EQUAL_FLOAT(0.0f, pid.compute(30.0f, 35.0f));    // отрицательное -> 0
    TEST_ASSERT_EQUAL_FLOAT(100.0f, pid.compute(100.0f, 0.0f));  // насыщение
}

void test_pid_integral_accumulates()
{
    PidController pid(0.0f, 1.0f, 0.0f, 2.0f, 0.0f, 100.0f);
    pid.compute(30.0f, 29.0f);                                    // +2
    TEST_ASSERT_EQUAL_FLOAT(4.0f, pid.compute(30.0f, 29.0f));     // +2 ещё раз
}

void test_pid_anti_windup()
{
    PidController pid(10.0f, 1.0f, 0.0f, 2.0f, 0.0f, 100.0f);
    for (int i = 0; i < 1000; i++)
    {
        pid.compute(40.0f, 20.0f); // выход давно в насыщении
    }
    TEST_ASSERT_TRUE(pid.getIntegral() <= 100.0f);
    // Как только ошибка сменила знак, выход должен сразу уйти из насыщения
    float out = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        out = pid.compute(40.0f, 45.0f);
    }
    TEST_ASSERT_TRUE(out < 100.0f);
}

void test_pid_derivative_on_measurement_no_kick()
{
    PidController pid(0.0f, 0.0f, 10.0f, 2.0f, 0.0f, 100.0f);
    pid.compute(30.0f, 30.0f);
    // Скачок уставки не должен давать выброс D (производная от измерения)
    TEST_ASSERT_EQUAL_FLOAT(0.0f, pid.compute(40.0f, 30.0f));
    // Рост измерения даёт отрицательный D, который прижимается к 0
    TEST_ASSERT_EQUAL_FLOAT(0.0f, pid.compute(40.0f, 31.0f));
}

void test_pid_reset_is_bumpless()
{
    PidController pid(8.0f, 0.08f, 0.0f, 2.0f, 0.0f, 100.0f);
    pid.reset(40.0f, 30.0f, 28.0f);
    TEST_ASSERT_EQUAL_FLOAT(40.0f, pid.getLastOutput());
    // Для той же ошибки первый шаг даёт выход, близкий к 40 % (+ вклад интеграла за 1 такт)
    const float out = pid.compute(30.0f, 28.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 40.0f, out);
}

void test_pid_set_tunings()
{
    PidController pid(1.0f, 0.0f, 0.0f, 2.0f, 0.0f, 100.0f);
    pid.setTunings(3.0f, 0.5f, 0.25f);
    TEST_ASSERT_EQUAL_FLOAT(3.0f, pid.getKp());
    TEST_ASSERT_EQUAL_FLOAT(0.5f, pid.getKi());
    TEST_ASSERT_EQUAL_FLOAT(0.25f, pid.getKd());
}

// ---------- Тепловая модель ----------
void test_thermal_model_idle_stays_at_ambient()
{
    ThermalModel m(AMBIENT_C, PLANT_MAX_C, PLANT_TAU_S, SAMPLE_S, PLANT_DELAY);
    for (int i = 0; i < 100; i++)
    {
        m.step(0.0f);
    }
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 22.0f, m.getTemperature());
}

void test_thermal_model_full_power_converges()
{
    ThermalModel m(AMBIENT_C, PLANT_MAX_C, PLANT_TAU_S, SAMPLE_S, PLANT_DELAY);
    for (int i = 0; i < 2000; i++)
    {
        m.step(100.0f);
    }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 70.0f, m.getTemperature());
}

void test_thermal_model_transport_delay()
{
    ThermalModel m(AMBIENT_C, PLANT_MAX_C, PLANT_TAU_S, SAMPLE_S, PLANT_DELAY);
    m.step(100.0f);
    m.step(100.0f);
    TEST_ASSERT_EQUAL_FLOAT(22.0f, m.getTemperature()); // 2 такта нагрев ещё "в пути"
    m.step(100.0f);
    TEST_ASSERT_TRUE(m.getTemperature() > 22.0f);
}

// ---------- Тревога и защита ----------
void test_alarm_requires_arming_and_delay()
{
    AlarmMonitor a(5.0f, 2.0f, 30000);
    TEST_ASSERT_FALSE(a.update(10.0f, 0)); // идёт разогрев: ещё не "вооружена"
    TEST_ASSERT_FALSE(a.update(10.0f, 60000));
    TEST_ASSERT_FALSE(a.update(1.0f, 61000)); // вошли в зону -> вооружена
    TEST_ASSERT_TRUE(a.isArmed());
    TEST_ASSERT_FALSE(a.update(6.0f, 62000)); // ошибка большая, но задержка не прошла
    TEST_ASSERT_FALSE(a.update(6.0f, 91999));
    TEST_ASSERT_TRUE(a.update(6.0f, 92000));  // 30 с -> тревога
    TEST_ASSERT_FALSE(a.update(1.0f, 93000)); // ошибка вернулась -> тревога снята
}

void test_alarm_disarmed_on_setpoint_change()
{
    AlarmMonitor a(5.0f, 2.0f, 1000);
    a.update(0.5f, 0);
    TEST_ASSERT_TRUE(a.isArmed());
    a.notifySetPointChanged();
    TEST_ASSERT_FALSE(a.isArmed());
    TEST_ASSERT_FALSE(a.update(8.0f, 5000));
}

void test_overtemp_guard_latches_with_hysteresis()
{
    OverTempGuard g(50.0f, 47.0f);
    TEST_ASSERT_FALSE(g.update(49.9f));
    TEST_ASSERT_TRUE(g.update(50.0f));
    TEST_ASSERT_TRUE(g.update(48.0f)); // ещё выше порога отпускания
    TEST_ASSERT_FALSE(g.update(47.0f));
}

// ---------- Разбор команд ----------
void test_command_parser_valid()
{
    Command c = parseCommand("sp 32.5");
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::SetPoint), static_cast<int>(c.type));
    TEST_ASSERT_EQUAL_FLOAT(32.5f, c.value);
    c = parseCommand("  KP   8 ");
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Kp), static_cast<int>(c.type));
    TEST_ASSERT_EQUAL_FLOAT(8.0f, c.value);
    c = parseCommand("auto");
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::ModeAuto), static_cast<int>(c.type));
    c = parseCommand("plot 0");
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Plot), static_cast<int>(c.type));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, c.value);
    c = parseCommand("status");
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Status), static_cast<int>(c.type));
}

void test_command_parser_invalid()
{
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::None), static_cast<int>(parseCommand("   ").type));
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Invalid), static_cast<int>(parseCommand("foo").type));
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Invalid), static_cast<int>(parseCommand("sp").type));
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Invalid), static_cast<int>(parseCommand("sp abc").type));
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Invalid), static_cast<int>(parseCommand("help 5").type));
    TEST_ASSERT_EQUAL(static_cast<int>(CommandType::Invalid), static_cast<int>(parseCommand("sp 3 4").type));
}

// ---------- Замкнутый контур ----------
void test_closed_loop_pid_reaches_setpoint()
{
    ThermalModel plant(AMBIENT_C, PLANT_MAX_C, PLANT_TAU_S, SAMPLE_S, PLANT_DELAY);
    PidController pid(8.0f, 0.08f, 10.0f, SAMPLE_S, 0.0f, 100.0f); // как в AppConfig.h
    float peak = 0.0f;
    for (int k = 0; k < 1500; k++)
    {
        const float out = pid.compute(30.0f, plant.getTemperature());
        plant.step(out);
        if (plant.getTemperature() > peak)
        {
            peak = plant.getTemperature();
        }
    }
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 30.0f, plant.getTemperature());
    TEST_ASSERT_TRUE(peak < 32.0f); // перерегулирование меньше 2 °C
}

int main(int, char**)
{
    UNITY_BEGIN();
    RUN_TEST(test_bounded_value_clamps_and_steps);
    RUN_TEST(test_pid_proportional_only);
    RUN_TEST(test_pid_integral_accumulates);
    RUN_TEST(test_pid_anti_windup);
    RUN_TEST(test_pid_derivative_on_measurement_no_kick);
    RUN_TEST(test_pid_reset_is_bumpless);
    RUN_TEST(test_pid_set_tunings);
    RUN_TEST(test_thermal_model_idle_stays_at_ambient);
    RUN_TEST(test_thermal_model_full_power_converges);
    RUN_TEST(test_thermal_model_transport_delay);
    RUN_TEST(test_alarm_requires_arming_and_delay);
    RUN_TEST(test_alarm_disarmed_on_setpoint_change);
    RUN_TEST(test_overtemp_guard_latches_with_hysteresis);
    RUN_TEST(test_command_parser_valid);
    RUN_TEST(test_command_parser_invalid);
    RUN_TEST(test_closed_loop_pid_reaches_setpoint);
    return UNITY_END();
}
