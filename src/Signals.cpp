#include "Signals.h"
#include <math.h>
#include "AppConfig.h"

BoundedValue setPoint(SET_POINT_DEFAULT_C, SET_POINT_MIN_C, SET_POINT_MAX_C, SET_POINT_STEP_C);
BoundedValue manualOutput(MANUAL_OUTPUT_DEFAULT_PERCENT, OUTPUT_MIN_PERCENT, OUTPUT_MAX_PERCENT,
                          MANUAL_OUTPUT_STEP_PERCENT);

float temperature = NAN;
float humidity = NAN;

float outputPercent = 0.0f;
float pidKp = PID_KP_DEFAULT;
float pidKi = PID_KI_DEFAULT;
float pidKd = PID_KD_DEFAULT;
bool autoMode = true;
bool overTempTripped = false;
bool alarmActive = false;
bool sensorFault = false;

bool plotMode = PLOT_MODE_DEFAULT;
