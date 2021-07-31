#include "SoundTask.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Critical.h"
#include "Pins.h"
#include "Parameters.h"
#include "Trace.h"

static WaitHandles::ValueHolder<SoundType> latch;

volatile unsigned long shutdownTime;

static bool CheckTime()
{
    return millis() >= shutdownTime;
}

static WaitHandles::Condition shutdown(CheckTime);

static const __FlashStringHelper* cat()
{
    return F("Sound");
}


TASK_BEGIN(SoundPlayer, { SoundType r; })

for (;;)
{
    pinMode(13, OUTPUT);

	TASK_WAIT_FOR(&latch);
	{
		Critical _;
		r = latch.Get();
	}

    Trace(cat(), F("Play"), (int)r);

    unsigned frequency;
    unsigned duration;

    switch (r)
    {
    case SoundType::None:
        shutdownTime = 0;        
        continue;
    case SoundType::Initialization1:
        duration = 200;
        frequency = 880;
        break;
    case SoundType::Initialization2:
        duration = 200;
        frequency = 988;
        break;
    case SoundType::Initialization3:
        duration = 200;
        frequency = 1109;
        break;

    case SoundType::MenuClick:
        duration = 20;
        frequency = 440;
        break;
    case SoundType::MenuSelected:
        duration = 100;
        frequency = 494;
        break;
    case SoundType::MenuServiceAlarm:
        duration = 1000;
        frequency = 349;
        break;

    case SoundType::PressureAlarm:
        duration = 200;
        frequency = 1244;
        break;

    case SoundType::StartMeasure:
        frequency = 880;
        duration = 200;
        break;

    case SoundType::MeasureFixed:
        frequency = 1109;
        duration = 200;
        break;
    case SoundType::MeasureComplete:
        frequency = 988;
        duration = 500;
        break;

    case SoundType::MeasureAborted:
        frequency = 784;
        duration = 500;
        break;


    default:
        continue;
    }

    Trace(cat(), F("SetBuzzer"));
    if (true)
    {
	    tone((uint8_t)Pins::Indication::Buzzer, frequency);
        digitalWrite(13, 1);
    }
    shutdownTime = millis() + duration;
    TASK_SLEEP(100);
}

TASK_END

TASK_BEGIN(SoundShutdownTask, {})
for(;;)
{
    TASK_WAIT_FOR(&shutdown);
    shutdownTime = -1;
    Trace(cat(), F("Shutdown"));
    noTone((uint8_t)Pins::Indication::Buzzer);
    digitalWrite(13, 0);
}
TASK_END

void PlaySound(SoundType type)
{
    Trace(cat(), F("Latch "), (int)type);
    latch.Set(type);
}

bool RegisterSoundTask(Scheduler & scheduler)
{
	return scheduler.Register(Instance<SoundPlayer>())
        && scheduler.Register(Instance<SoundShutdownTask>());
}
