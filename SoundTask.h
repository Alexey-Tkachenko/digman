#ifndef _SOUND_H_
#define _SOUND_H_

#include "Scheduler.h"

enum class SoundType : byte
{
    None,

    Initialization1,
    Initialization2,
    Initialization3,

    MenuClick,
    MenuSelected,
    MenuServiceAlarm,

    PressureAlarm,

    StartMeasure,
    MeasureFixed,
    MeasureComplete,
    MeasureAborted,
};


void PlaySound(SoundType type);

bool RegisterSoundTask(Scheduler& scheduler);

#endif
