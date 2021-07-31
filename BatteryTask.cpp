#include <Arduino.h>
#include "BatteryTask.h"
#include "PeriodicTimer.h"
#include "Scheduler.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Pins.h"
#include "LcdIo.h"
#include "Globals.h"

TASK_BEGIN(BatteryTask, { int value; byte batteryLevel{}; byte oldBatteryLevel{};})

pinMode(uint8_t(Pins::Telemetry::Voltage), INPUT);
digitalWrite(uint8_t(Pins::Telemetry::Voltage), 1);

for (;;)
{
    value = analogRead(uint8_t(Pins::Telemetry::Voltage));
    oldBatteryLevel = batteryLevel;
    batteryLevel = GetBatteryLevel(value);
    if (oldBatteryLevel != batteryLevel)
    {
        UpdateBatteryChar(batteryLevel);
    }

    TASK_SLEEP(10 SECONDS);
}

TASK_BODY_END

static void UpdateBatteryChar(byte level)
{
    uint8_t rows[8];
    rows[0] = 0b01110;
    rows[1] = level > 5 ? 0b11111 : 0b10001;
    rows[2] = level > 4 ? 0b11111 : 0b10001;
    rows[3] = level > 3 ? 0b11111 : 0b10001;
    rows[4] = level > 2 ? 0b11111 : 0b10001;
    rows[5] = level > 1 ? 0b11111 : 0b10001;
    rows[6] = level > 0 ? 0b11111 : 0b10001;
    rows[7] = 0b11111;

    lcdSetCharShape(BATTERY_CHAR, rows);
}

static byte GetBatteryLevel(int value)
{
    if (value < 730)
    {
        return 0;
    }
    if (value < 750)
    {
        return 1;
    }
    if (value < 770)
    {
        return 2;
    }
    if (value < 790)
    {
        return 3;
    }
    if (value < 810)
    {
        return 4;
    }
    if (value < 830)
    {
        return 5;
    }
    return 6;
}

TASK_CLASS_END

bool RegisterBatteryTask(Scheduler& scheduler)
{
    return scheduler.Register(Instance<BatteryTask>(), TaskPriority::RealTime);
}
