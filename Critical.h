#ifndef _CRITICA_H_
#define _CRITICA_H_

#include "Arduino.h"

class Critical
{
public:
	Critical()
	{
		state = SREG;
		cli();
	}
	~Critical()
	{
		SREG = state;
	}
private:
	byte state;
};

#endif
