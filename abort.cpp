#include "Arduino.h"

const __FlashStringHelper* AssertMessage;

void prog_abort()
{
	if (AssertMessage)
	{
		Serial.println(AssertMessage);
	}
	digitalWrite(13, 1);
	for (;;);
}
