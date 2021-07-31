#include <Arduino.h>
#include "BarometerTask.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "WaitHandles.h"
#include "Pins.h"
#include "Parameters.h"
#include "Median.h"
#include "Trace.h"

static bool startMeasure{false};
static bool abortMeasure { false };

static constexpr int NotMeasured = -1;
static constexpr int MeasureAborted = -2;

static int valueMeasured{ NotMeasured };
static Duration durationMeasured{};
static Duration duration{};
static Duration durationPart{};

static const __FlashStringHelper* cat()
{
    return F("Pressure");
}


static int pressure{};
static int maxPressure{};

TASK_BEGIN(BarometerTask, { int value{}; unsigned long counter{}; unsigned long totalDuration{}; bool aboveLimit{};  })

analogReference(EXTERNAL);

for(;;)
{
    m.fill(0);
    maxPressure = 0;
    pressure = 0;

    Trace(cat(), F("Pending"));
    while (startMeasure == false)
    {
        TASK_SLEEP(100);
    }
    Trace(cat(), F("Started"));
    startMeasure = false;
    abortMeasure = false;

    duration = 0;
    aboveLimit = false;
    durationPart = 0;

    totalDuration = millis();
    for(counter = 0;;++counter)
    {
        TASK_YIELD();
        if (abortMeasure)
        {
            abortMeasure = false;
            valueMeasured = MeasureAborted;
            break;
        }

        value = analogRead((uint8_t)Pins::Telemetry::Pressure);
        m.write(value);
        pressure = static_cast<int>(0.5 + (m.get() - Parameters::PressureOffset) * Parameters::PressureScale);
        if (pressure > maxPressure)
        {
            maxPressure = pressure;
        }

        if (pressure > Parameters::PressureThreshold)
        {
            if (aboveLimit == false)
            {
                if (!durationPart)
                {
                    durationPart = millis();
                }
            }
            aboveLimit = true;
        }
        else
        {
            if (aboveLimit == true)
            {
                if (durationPart)
                {
                    duration += millis() - durationPart;
                    durationPart = 0;
                }
            }
            aboveLimit = false;
        }
        
        if (maxPressure > BarometerLimit && pressure < maxPressure / 2)
        {
            if (durationPart)
            {
                duration += (millis() - durationPart);
                durationPart = 0;
            }

            valueMeasured = maxPressure;
            durationMeasured = duration;
            break;
        }
    }
    totalDuration = millis() - totalDuration;

    Trace(cat(), F("Finished"));
    Trace(cat(), F("Dur"), totalDuration);
    Trace(cat(), F("Dur"), counter);
}

TASK_BODY_END

Median<int, 7, byte> m;

TASK_CLASS_END

void StartMeasurement()
{
    Trace(cat(), F("Start requested"));
    startMeasure = true;
    valueMeasured = NotMeasured;
}

bool AbortMeasurement()
{
    Trace(cat(), F("Abort requested"));
    if (abortMeasure)
    {
        return false;
    }
    abortMeasure = true;
    return true;
}

bool GetMeasurement(int& value, Duration& duration)
{
    value = valueMeasured;
    duration = durationMeasured;
    return value >= 0;
}

void GetMeasurementState(int& current, int& peak, Duration& duration)
{
    current = pressure;
    peak = maxPressure;
    duration = ::duration;

    if (::durationPart)
    {
        duration += millis() - ::durationPart;
    }
}

bool RegisterBarometerTask(Scheduler& scheduler)
{
    return scheduler.Register(Instance<BarometerTask>(), TaskPriority::SensorPoll);
}
