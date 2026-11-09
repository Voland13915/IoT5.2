#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

// ---------- Пины (Arduino Uno) ----------
constexpr uint8_t DHT_PIN = 2;             // DHT22, линия данных (подтяжка 10 кОм к +5 В)
constexpr uint8_t BUTTON_UP_PIN = 4;       // уставка (AUTO) или выход (MAN) вверх
constexpr uint8_t BUTTON_DOWN_PIN = 5;     // уставка (AUTO) или выход (MAN) вниз
constexpr uint8_t BUTTON_MODE_PIN = 6;     // переключение AUTO / MAN
constexpr uint8_t BUZZER_PIN = 8;          // buzzer (tone, Timer2)
constexpr uint8_t HEATER_PWM_PIN = 9;      // ШИМ на затвор MOSFET (Timer1, ~490 Гц)
constexpr uint8_t ALARM_LED_PIN = 10;      // красный светодиод тревоги
constexpr uint8_t STATUS_LED_PIN = 13;     // зелёный светодиод: выход больше 0
// LCD по I2C: SDA = A4, SCL = A5 (выводы аппаратного TWI)

// ---------- Последовательный порт и дисплей ----------
constexpr unsigned long SERIAL_BAUD_RATE = 9600;
constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

// ---------- Периоды (мс) ----------
constexpr uint32_t CONTROL_PERIOD_MS = 2000;   // DHT22 опрашивается не чаще 0,5 Гц
constexpr uint32_t DISPLAY_PERIOD_MS = 250;
constexpr uint32_t BUTTON_POLL_PERIOD_MS = 20; // постоянный период опроса для антидребезга
constexpr float CONTROL_PERIOD_S = CONTROL_PERIOD_MS / 1000.0f;

// ---------- Кнопки ----------
constexpr uint8_t BUTTON_DEBOUNCE_SAMPLES = 2;
// Автоповтор при удержании: через 25 * 20 мс = 500 мс, далее каждые 8 * 20 мс = 160 мс
constexpr uint8_t BUTTON_REPEAT_DELAY_SAMPLES = 25;
constexpr uint8_t BUTTON_REPEAT_PERIOD_SAMPLES = 8;

// ---------- Уставка (°C) ----------
constexpr float SET_POINT_DEFAULT_C = 30.0f;
constexpr float SET_POINT_MIN_C = 15.0f;
constexpr float SET_POINT_MAX_C = 45.0f;
constexpr float SET_POINT_STEP_C = 0.5f;

// ---------- PID ----------
// Начальные значения подобраны на модели объекта; на макете уточняются командами kp, ki, kd
constexpr float PID_KP_DEFAULT = 8.0f;     // %/°C
constexpr float PID_KI_DEFAULT = 0.08f;    // %/(°C·с), постоянная интегрирования Ti = Kp/Ki = 100 с
constexpr float PID_KD_DEFAULT = 10.0f;    // %·с/°C
constexpr float OUTPUT_MIN_PERCENT = 0.0f;
constexpr float OUTPUT_MAX_PERCENT = 100.0f;

// ---------- Ручной режим ----------
constexpr float MANUAL_OUTPUT_DEFAULT_PERCENT = 0.0f;
constexpr float MANUAL_OUTPUT_STEP_PERCENT = 5.0f;

// ---------- Тревога и защита ----------
constexpr float ALARM_ERROR_C = 5.0f;              // "большая ошибка"
constexpr float ALARM_ARM_BAND_C = 2.0f;           // тревога включается после входа в эту зону
constexpr uint32_t ALARM_DELAY_MS = 30000;
constexpr float OVER_TEMP_LIMIT_C = 50.0f;         // аварийное отключение нагревателя
constexpr float OVER_TEMP_RELEASE_C = 47.0f;
constexpr uint16_t BUZZER_FREQUENCY_HZ = 2000;
constexpr uint32_t ALARM_BLINK_PERIOD_MS = 500;

// ---------- Датчик ----------
constexpr uint32_t SENSOR_MIN_INTERVAL_MS = 2000;
constexpr uint8_t SENSOR_MAX_FAILURES = 3;         // столько неудачных чтений подряд = авария датчика

// ---------- Вывод ----------
constexpr bool PLOT_MODE_DEFAULT = true;           // true = формат Arduino Serial Plotter

// ---------- Тепловая модель (только сборка uno_sim, -DSIMULATE_PLANT) ----------
constexpr float AMBIENT_TEMP_C = 22.0f;
constexpr float PLANT_MAX_TEMP_C = 70.0f;          // установившаяся температура при 100 % мощности
constexpr float PLANT_TIME_CONSTANT_S = 180.0f;
constexpr uint8_t PLANT_DELAY_SAMPLES = 2;
constexpr float SIMULATED_HUMIDITY_PERCENT = 50.0f;

#endif
