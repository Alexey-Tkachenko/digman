#pragma once
#include "Scheduler.h"

enum class PressedButton : unsigned char
{
    None = 0,
    Start = 1,
    Up = 2,
    Down = 4
};

inline bool operator&&(PressedButton a, PressedButton b)
{
    return static_cast<unsigned char>(a) & static_cast<unsigned char>(b);
}

inline PressedButton operator| (PressedButton a, PressedButton b)
{
    return PressedButton(static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
}

inline PressedButton& operator|= (PressedButton &a, PressedButton b)
{
    return a = PressedButton(static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
}

inline PressedButton& operator&= (PressedButton &a, PressedButton b)
{
    return a = PressedButton(static_cast<unsigned char>(a)& static_cast<unsigned char>(b));
}

inline PressedButton operator~ (PressedButton a)
{
    return PressedButton(~static_cast<unsigned char>(a));
}


bool RegisterButtonTask(Scheduler& scheduler);

bool GetPressedButtons(PressedButton& buttons);
