#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>

namespace Pins
{
    enum class Control : uint8_t
    {
        ButtonStart = 4,
        ButtonDown = 5,
        ButtonUp = 6,
    };

    enum class Indication : uint8_t
    {
        Buzzer = 11,
    };

    enum class Telemetry : uint8_t
    {
        Voltage = A0,
        Pressure = A1,
    };

    enum class I2C : byte
    {
        LCD = 0x3F,
    };

}
