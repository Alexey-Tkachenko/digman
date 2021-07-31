#ifndef _WAIT_OBJECT_
#define _WAIT_OBJECT_

typedef struct WaitObject
{
	virtual bool IsSignalled() = 0; 
} *WaitHandle;


#endif