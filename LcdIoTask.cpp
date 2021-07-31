#include "LcdIoTask.h"
#include "Arduino.h"

#include "ExpressTask.h"
#include "Globals.h"
#include "StaticAllocActivator.h"
#include "LcdIo.h"
#include "WString.h"

WaitHandles::DataQueue<LcdCommand, 8> lcdCommandQueue;

TASK_BEGIN(LcdIoTask, { } )

lcdInit();

for (;;)
{
    TASK_WAIT_FOR(&lcdCommandQueue);

    while (lcdCommandQueue.Size())
    {
        const LcdCommand& cmd = lcdCommandQueue.Get();

        switch (cmd.Code)
        {
        case LcdCommandCode::None:
            break;
        case LcdCommandCode::Clear:
            lcdClear();
            break;
        case LcdCommandCode::Write:
            lcdPrint(cmd.LineA, cmd.LineB);
            break;
        case LcdCommandCode::WriteKeep:
            lcdPrint(cmd.LineA, cmd.LineB, false);
            break;
        case LcdCommandCode::WriteFlash:
            lcdPrint(cmd.LineAFlash, cmd.LineBFlash);
            break;
        case LcdCommandCode::WriteKeepFlash:
            lcdPrint(cmd.LineAFlash, cmd.LineBFlash, false);
            break;
        case LcdCommandCode::WriteAt:
            lcdPutLineAt(cmd.Row, cmd.Column, cmd.Line);
            break;
        case LcdCommandCode::WriteAtFlash:
            lcdPutLineAt(cmd.Row, cmd.Column, cmd.LineFlash);
            break;
        case LcdCommandCode::CharAt:
            lcdPutCharAt(cmd.Row, cmd.Column, cmd.Char);
            break;
        case LcdCommandCode::Backlight:
            lcdSetBacklight(cmd.BacklightValue);
            break;
        }
    }

    TASK_YIELD();
}
TASK_END;

void RegisterLcdIoTask(Scheduler &scheduler)
{
    scheduler.Register(Instance<LcdIoTask>(), TaskPriority::RealTime);
}

bool LcdCommandEnqueue(const LcdCommand & command)
{
    return ::lcdCommandQueue.Put(command);
}

static LcdCommand InitStaticCommand(LcdCommandCode code)
{
    LcdCommand cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.Code = code;
    return cmd;
}

template<LcdCommandCode Code>
const LcdCommand& GenericCommand()
{
    static byte cmd[sizeof(LcdCommand)];
    static LcdCommand* ptr;
    if (!ptr)
    {
        ptr = new(cmd)LcdCommand();
        *ptr = InitStaticCommand(Code);
    }
    return *ptr;
}

const LcdCommand & LcdCommand::None()
{
    return GenericCommand<LcdCommandCode::None>();
}

const LcdCommand & LcdCommand::Clear()
{
    return GenericCommand<LcdCommandCode::Clear>();
}

LcdCommand LcdCommand::Write(const char * first, const char * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::Write;
    cmd.LineA = first;
    cmd.LineB = second;
    return cmd;
}

LcdCommand LcdCommand::Write(const __FlashStringHelper * first, const __FlashStringHelper * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteFlash;
    cmd.LineAFlash = first;
    cmd.LineBFlash = second;
    return cmd;
}

LcdCommand LcdCommand::WriteKeep(const char * first, const char * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteKeep;
    cmd.LineA = first;
    cmd.LineB = second;
    return cmd;
}

LcdCommand LcdCommand::WriteKeep(const __FlashStringHelper * first, const __FlashStringHelper * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteKeepFlash;
    cmd.LineAFlash = first;
    cmd.LineBFlash = second;
    return cmd;;
}

LcdCommand LcdCommand::WriteAt(byte row, byte column, const char * ptr)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteAt;
    cmd.Row = row;
    cmd.Column = column;
    cmd.Line = ptr;
    return cmd;
}

LcdCommand LcdCommand::WriteAt(byte row, byte column, const __FlashStringHelper * ptr)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteAtFlash;
    cmd.Row = row;
    cmd.Column = column;
    cmd.LineFlash = ptr;
    return cmd;
}

LcdCommand LcdCommand::CharAt(byte row, byte column, char c)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::CharAt;
    cmd.Row = row;
    cmd.Column = column;
    cmd.Char = c;
    return cmd;
}

LcdCommand LcdCommand::Backlight(bool value)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::Backlight;
    cmd.BacklightValue = value;
    return cmd;
}

void LcdCommand::Enqueue() const
{
    LcdCommandEnqueue(*this);
}
