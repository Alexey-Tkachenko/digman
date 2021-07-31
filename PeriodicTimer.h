#pragma once
#include "WaitObject.h"

class PeriodicTimer : public WaitObject
{
private:
    unsigned long interval;
    unsigned long nextTick;
    bool running;
public:
    PeriodicTimer(unsigned long intervalMilliseconds, bool start)
        : interval(intervalMilliseconds), nextTick(), running()
    {
        if (start)
        {
            Start();
        }
    }

    void Start()
    {
        nextTick = millis() + interval;
        running = true;
    }

    void Stop()
    {
        running = false;
    }

    virtual bool IsSignalled() override
    {
        if (!running)
        {
            return false;
        }

        unsigned long now = millis();
        if (nextTick <= now)
        {
            while (nextTick <= now)
            {
                nextTick += interval;
            }
            return true;
        }

        return false;
    }
};