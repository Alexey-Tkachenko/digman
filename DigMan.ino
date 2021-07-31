#include <EEPROM.h>
#include "Scheduler.h"

#include "SoundTask.h"
#include "LcdIoTask.h"
#include "ButtonTask.h"
#include "BarometerTask.h"
#include "WorkerTask.h"
#include "BatteryTask.h"

void setup() __ATTR_NORETURN__;

static Scheduler scheduler;


void setup()
{
    Serial.begin(115200);

    RegisterSoundTask(scheduler);
    RegisterLcdIoTask(scheduler);
    RegisterButtonTask(scheduler);
    RegisterBarometerTask(scheduler);
    RegisterWorkerTask(scheduler);
    RegisterBatteryTask(scheduler);

    for (;;)
    {
        scheduler.Invoke();
    }
}

void loop()
{
}
