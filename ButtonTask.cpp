#include "ButtonTask.h"
#include "ExpressTask.h"
#include "Pins.h"
#include "Trace.h"
#include "StaticAllocActivator.h"
#include "Globals.h"

static PressedButton PressedButtons;

TASK_BEGIN(ButtonTask, {})

pinMode(pin, INPUT);
digitalWrite(pin, 1);

for (;;)
{
    if (digitalRead(pin) == 1)
    {
        TASK_SLEEP(100);
        continue;
    }

    TASK_SLEEP(100);

    if (digitalRead(pin) == 0)
    {
        TASK_YIELD_WHILE(digitalRead(pin) == 0);
        PressedButtons |= button;
        Trace(F("Button"), F("pressed"), pin);
    }
}

TASK_BODY_END


uint8_t pin;
PressedButton button;

public:

ButtonTask(Pins::Control pin, PressedButton & button) : pin((uint8_t)pin), button(button) {}

TASK_CLASS_END;

bool RegisterButtonTask(Scheduler& scheduler)
{
    return true
    & scheduler.Register(Instance<ButtonTask, 0, Pins::Control, PressedButton>(Pins::Control::ButtonStart, PressedButton::Start), TaskPriority::RealTime)
    & scheduler.Register(Instance<ButtonTask, 1, Pins::Control, PressedButton>(Pins::Control::ButtonUp, PressedButton::Up), TaskPriority::RealTime)
    & scheduler.Register(Instance<ButtonTask, 2, Pins::Control, PressedButton>(Pins::Control::ButtonDown, PressedButton::Down), TaskPriority::RealTime)
    ;
}

bool GetPressedButtons(PressedButton& buttons)
{
    buttons = ::PressedButtons;
    ::PressedButtons = PressedButton::None;
    return buttons != PressedButton::None;
}
