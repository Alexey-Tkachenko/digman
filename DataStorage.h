#pragma once
#ifndef _DATA_STORAGE_H_
#define _DATA_STORAGE_H_
#include <Arduino.h>

struct DataItem
{
    uint8_t  Counter;
    int16_t  MaxValue;
    uint16_t Duration;
};

static constexpr int32_t DataItemDelimiter = -1;
static constexpr uint8_t DataItemCounterEmptyValue = 0;
static constexpr uint8_t DataItemCounterResetValue = 1;
static constexpr uint8_t DataItemCounterToReset = 255;

constexpr byte DataItemsCount = (sizeof(DataItem) * 255 < E2END + 1) ? 255 : (E2END + 1) / sizeof(DataItem);

void ResetDataItems();

DataItem ReadDataItem(byte index);
byte ReadDataItemCounter(byte index);
void WriteDataItemCounter(byte index, byte value);

bool WriteDataItem(byte& index, const DataItem& item);
void UpdateDataItemsCounters();

byte FindHeadDataItemIndex();

inline byte NextCounter(byte counter)
{
    return counter + 1;
}


#endif

