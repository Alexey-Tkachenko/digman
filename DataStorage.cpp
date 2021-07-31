#include "DataStorage.h"
#include <EEPROM.h>

static size_t GetItemOffset(byte index)
{
    return index * sizeof(DataItem);
}

void ResetDataItems()
{
    for (decltype(EEPROM.length()) i = 0; i < EEPROM.length(); ++i)
    {
        EEPROM[i].update(0);
    }
}

DataItem ReadDataItem(byte index)
{
    DataItem result;
    EEPROM.get(GetItemOffset(index), result);
    return result;
}

byte ReadDataItemCounter(byte index)
{
    return EEPROM[GetItemOffset(index) + offsetof(DataItem, Counter)];
}

void WriteDataItemCounter(byte index, byte value)
{
    EEPROM[GetItemOffset(index) + offsetof(DataItem, Counter)] = value;
}

bool WriteDataItem(byte& index, const DataItem& item)
{
    EEPROM.put(GetItemOffset(index), item);
    if (++index == DataItemsCount)
    {
        index = 0;
        if (item.Counter == DataItemCounterToReset)
        {
            UpdateDataItemsCounters();
            return true;
        }
    }
    return false;
}

void UpdateDataItemsCounters()
{
    for (byte i = 0; i < DataItemsCount; ++i)
    {
        EEPROM[GetItemOffset(i) + offsetof(DataItem, Counter)] = DataItemCounterResetValue;
    }
}

byte FindHeadDataItemIndex()
{
    byte index;
    const byte initial = ReadDataItem(index = 0).Counter;
    if (initial == 0) return 0;

    for (byte i = 1; i < DataItemsCount; ++i)
    {
        if (ReadDataItem(i).Counter != initial)
        {
            return i;
        }
    }

    return 0;
}

