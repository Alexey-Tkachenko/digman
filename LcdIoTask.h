#ifndef _LCD_IO_TASK_H_
#define _LCD_IO_TASK_H_

#include "Scheduler.h"

void RegisterLcdIoTask(Scheduler& scheduler);

enum struct LcdCommandCode : byte
{
    None,
    Clear,
    Write,
    WriteFlash,
    WriteKeep,
    WriteKeepFlash,
    WriteAt,
    WriteAtFlash,
    CharAt,
    Backlight,
};

struct LcdCommand 
{
    LcdCommandCode Code;
    union
    {
        const __FlashStringHelper* LineAFlash;
        const char* LineA;
        char Char;
        const char* Line;
        const __FlashStringHelper* LineFlash;
        bool BacklightValue;
        byte BacklightPwmValue;
    };
    union
    {
        const __FlashStringHelper* LineBFlash;
        const char* LineB;
        struct
        {
            byte Row;
            byte Column;
        };
    };

    static const LcdCommand& None();
    static const LcdCommand& Clear();
    static LcdCommand Write(const char* first, const char* second = nullptr);
    static LcdCommand Write(const __FlashStringHelper*  first, const __FlashStringHelper* second = nullptr);
    static LcdCommand WriteKeep(const char* first, const char* second = nullptr);
    static LcdCommand WriteKeep(const __FlashStringHelper*  first, const __FlashStringHelper* second = nullptr);
    static LcdCommand WriteAt(byte row, byte column, const char* ptr);
    static LcdCommand WriteAt(byte row, byte column, const __FlashStringHelper* ptr);
    static LcdCommand CharAt(byte row, byte column, char c);
    static LcdCommand Backlight(bool value);

    void Enqueue() const;
};

bool LcdCommandEnqueue(const LcdCommand& command);

#endif

