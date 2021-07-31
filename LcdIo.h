#ifndef _LCD_IO_H_
#define _LCD_IO_H_

#include "Arduino.h"
#include "WString.h"

void lcdPrint(const char* a = 0, const char* b = 0, bool clear = true);

void lcdPrint(const  __FlashStringHelper * a = 0, const __FlashStringHelper* b = 0, bool clear = true);

void lcdClear();

void lcdClearLine(byte line);

void lcdInit();

void lcdPutCharAt(byte row, byte col, char symbol);

void lcdPutLineAt(byte row, byte col, const char* symbols);

void lcdPutLineAt(byte row, byte col, const __FlashStringHelper* symbols);

void lcdSetBacklight(bool value);

void lcdSetCharShape(byte index, byte data[8]);

#endif
