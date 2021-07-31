#pragma once

#include <Arduino.h>

//#ifdef USE_TRACE

inline bool TraceCommon(const __FlashStringHelper* cat, const __FlashStringHelper* str)
{
    //if (strcmp_P("Sound", (const char*)cat) == 0)
    //{
    //    return false;
    //}

    Serial.print(millis());
    Serial.print(F(" :\t"));
    Serial.print(cat);
    Serial.print(F("\t"));
    Serial.print(str);

    return true;
}

inline void Trace(const __FlashStringHelper* cat, const __FlashStringHelper* str)
{
    if (TraceCommon(cat, str))
    {
        Serial.println();
    }
}

template<class T>
void Trace(const __FlashStringHelper* cat, const __FlashStringHelper* str, const T& arg)
{
    if (TraceCommon(cat, str))
    {
        Serial.write('\t');
        Serial.println(arg);
    }
}
//
//#else
//
//inline void Trace(const __FlashStringHelper* cat, const __FlashStringHelper* str){}
//template<class T>
//void Trace(const __FlashStringHelper* cat, const __FlashStringHelper* str, const T& arg){}
//
//#endif