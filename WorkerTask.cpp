#include "WorkerTask.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "SoundTask.h"
#include "LcdIoTask.h"
#include "ButtonTask.h"
#include "BarometerTask.h"
#include "Trace.h"
#include "BatteryTask.h"
#include "Globals.h"
#include "DataStorage.h"
#include "LcdIo.h"
#include "Parameters.h"


static const __FlashStringHelper* cat()
{
    return F("Worker");
}

constexpr int SpecialMenuItems = 3;

constexpr int SessionDelimiter = -666;

TASK_BEGIN(WorkerTask,
    {
        PressedButton buttons{PressedButton::None};

        int measuredValue{};
        bool aborted{};
        int current{};
        int peak{};
        Duration duration{};

        int menuIndex{};

        byte usedItems{};
        byte writePosition{};
    })
RestartTask:

    LcdCommandEnqueue(LcdCommand::Clear());
    TASK_SLEEP(1 SECOND);

    menuIndex = 0;

    ScanCounters();

    {
        byte data[8] = {
            B11111,
            B11011,
            B10001,
            B10001,
            B11011,
            B11111,
            B11011,
            B11111,
        };
        lcdSetCharShape(DANGEROUS_CHAR, data);
    }

PutBatteryStatus();
PlaySound(SoundType::Initialization1);
TASK_SLEEP(300);
PlaySound(SoundType::Initialization2);
TASK_SLEEP(300);
PlaySound(SoundType::Initialization3);
TASK_SLEEP(600);
ShowMenuItem(menuIndex);

for (;;)
{
    Trace(cat(), F("Waiting"));
    for (;;)
    {
        if (GetPressedButtons(buttons))
        {
            break;
        }
        TASK_SLEEP(100);
    }

    LcdCommandEnqueue(LcdCommand::Clear());

    if (buttons && PressedButton::Start)
    {
        switch (menuIndex)
        {
        case -1:
            PutValue(SessionDelimiter, Duration{});
            ShowMenuItem(menuIndex = 0);
            PlaySound(SoundType::MenuClick);
            continue;
        case -2:
            PlaySound(SoundType::MenuServiceAlarm);
            break;
        case -3:
            ResetDataItems();
            PlaySound(SoundType::MenuServiceAlarm);
            goto RestartTask;
        default:
            PlaySound(SoundType::MenuClick);
            break;
        }

        Trace(cat(), F("Started"));

        AbortMeasurement();
        PlaySound(SoundType::StartMeasure);
        fixed = false;
        StartMeasurement();

        TASK_SLEEP(200);

        aborted = false;
        while (true)
        {
            if (GetPressedButtons(buttons))
            {
                PlaySound(SoundType::MenuSelected);
                AbortMeasurement();
                ShowState(true, current, peak, duration);
                break;
            }

            if (GetMeasurement(measuredValue, duration))
            {
                PlaySound(SoundType::MeasureComplete);
                PutValue(measuredValue, duration);
                ShowMenuItem(menuIndex = 0);
                break;
            }

            GetMeasurementState(current, peak, duration);
            ShowState(false, current, peak, duration);

            TASK_YIELD();
        }
    }

    if (buttons && (PressedButton::Down | PressedButton::Up))
    {
        Trace(cat(), F("Control"));

        PlaySound(SoundType::MenuClick);

        if (buttons && PressedButton::Down)
        {
            ++menuIndex;

            if (menuIndex == -2) { ++menuIndex; }

            if (menuIndex >= int(usedItems))
            {
                menuIndex = 0;
            }
        }
        else if (buttons && PressedButton::Up)
        {
            if (menuIndex > -SpecialMenuItems)
            {
                --menuIndex;
            }
        }
        ShowMenuItem(menuIndex);
    }

    TASK_YIELD();
}

TASK_BODY_END

static void ShowNoData()
{
    LcdCommandEnqueue(LcdCommand::Write(F("  No data")));
    PutBatteryStatus();
}

void ScanCounters()
{
    byte c = ReadDataItemCounter(0);
    if (c == DataItemCounterEmptyValue)
    {
        writePosition = 0;
        usedItems = 0;
    }
    else
    {
        writePosition = 0;
        usedItems = DataItemsCount;

        for (byte i = 1; i < DataItemsCount; ++i)
        {
            byte c2 = ReadDataItemCounter(i);
            if (c2 != c)
            {
                writePosition = i;
                if (c2 == DataItemCounterEmptyValue)
                {
                    usedItems = i;
                }
                break;
            }
        }
    }
}

static void PutBatteryStatus() 
{
    LcdCommandEnqueue(LcdCommand::CharAt(1, 15, BATTERY_CHAR));
}

mutable bool fixed;

void ShowState(bool aborted, int current, int peak, Duration duration) const
{
    static char first[17];
    static char second[17];

    static const char strAborted[17] = "   <Aborted>    ";

    snprintf_P(first, sizeof(first), PSTR("Cur %3d Pk %3d %c"), current, peak, peak > BarometerLimit ? '*' : ' ');

    if (!fixed && peak > BarometerLimit)
    {
        fixed = true;
        PlaySound(SoundType::MeasureFixed);
    }

    if (aborted)
    {
        PlaySound(SoundType::MeasureAborted);
        LcdCommandEnqueue(LcdCommand::WriteKeep(first,strAborted));
    }
    else
    {
        auto saved = SaveDuration(duration);
        snprintf_P(second, sizeof(second), PSTR(">%umm %3u.%us"), Parameters::PressureThreshold, saved / 10, saved % 10);
        LcdCommandEnqueue(LcdCommand::WriteKeep(first, second));
    }

    if (current > Parameters::PressureThreshold)
    {
        PlaySound(SoundType::PressureAlarm);
    }
    
    PutBatteryStatus();
}

void ShowMenuItem(int index) const
{
    Trace(cat(), F("Meas"), index);
    Trace(cat(), F("Used"), usedItems);

    static char bufferA[17];
    static char bufferB[17];

    switch (index)
    {
    case -1:
        LcdCommandEnqueue(LcdCommand::Write(F("Start new"), F("session")));
        break;
    case -2:
        PlaySound(SoundType::MenuServiceAlarm);
        LcdCommandEnqueue(LcdCommand::Write(
            F("\2\2 DANGEROUS \2\2"), 
            F("\2\2   ZONE    \2\2")));
        break;
    case -3:
        LcdCommandEnqueue(LcdCommand::Write(F("Clear all data"), F("permanently")));
        break;
    default:
        PlaySound(SoundType::MenuSelected);
        if (usedItems == 0)
        {
            ShowNoData();
        }
        else if (GetItemString(index, bufferA, sizeof(bufferA)))
        {
            if (GetItemString(index + 1, bufferB, sizeof(bufferB)))
            {
                LcdCommandEnqueue(LcdCommand::Write(bufferA, bufferB));
            }
            else
            {
                LcdCommandEnqueue(LcdCommand::Write(bufferA));
            }
        }
        break;
    }
    
    PutBatteryStatus();
}

bool GetItemString(byte index, char* buffer, int size) const
{
    int peak;
    uint16_t duration;
    if (GetValue(byte(index), peak, duration))
    {
        if (peak == SessionDelimiter)
        {
            snprintf_P(buffer, size, PSTR("%3u -----------"), index + 1);
        }
        else
        {
            snprintf_P(buffer, size, PSTR("%3u %3umm %2u.%us"), index + 1, peak, duration / 10, duration % 10);
        }
        return true;
    }
    return false;
}

static uint16_t SaveDuration(Duration duration)
{
    return (duration + 50) / 100;
}

void PutValue(int peak, Duration duration)
{
    if (usedItems < DataItemsCount)
    {
        ++usedItems;
    }

    DataItem item;
    item.Counter = NextCounter(ReadDataItemCounter(writePosition));
    item.MaxValue = peak;
    item.Duration = SaveDuration(duration);
    WriteDataItem(writePosition, item);
}

bool GetValue(byte index, int& peak, uint16_t& displayDuration) const
{
    if (index > usedItems - 1)
    {
        return false;
    }
    int realPosition = writePosition - 1 - index;
    while (realPosition < 0) realPosition += DataItemsCount;
    DataItem item = ReadDataItem(realPosition);
    peak = item.MaxValue;
    displayDuration = item.Duration;
    return true;
}

TASK_CLASS_END


bool RegisterWorkerTask(Scheduler& scheduler)
{
    return scheduler.Register(Instance<WorkerTask>());
}
