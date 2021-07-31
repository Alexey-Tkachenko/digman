#pragma once
#include "Scheduler.h"

using Duration = uint_fast32_t;

bool RegisterBarometerTask(Scheduler&);

void StartMeasurement();
bool AbortMeasurement();
bool GetMeasurement(int& value, Duration& duration);
void GetMeasurementState(int& current, int& peak, Duration& duration);

constexpr int BarometerLimit = 50;
