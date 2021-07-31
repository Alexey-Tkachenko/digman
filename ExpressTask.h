#ifndef _EXPRESS_TASK_
#define _EXPRESS_TASK_

#include "TaskBase.h"
#include "WaitObject.h"
#include "Arduino.h"

#define TASK_CLASS(TypeName) TypeName

#define TASK_BEGIN(TypeName, Locals) class TASK_CLASS(TypeName) : public StatefullTaskBase { \
	struct Locals; \
	virtual bool Step() override { \
switch (this->state) {	\
case -1: return true; \
case 0:


#define TASK_BODY_END ;} return true; }
#define TASK_CLASS_END };

#define TASK_END TASK_BODY_END TASK_CLASS_END

#define TASK_YIELD() this->state = __LINE__; return false; case __LINE__:

#define TASK_WAIT_FOR(Object) this->WaitFor(Object); this->state = __LINE__; return false; case __LINE__:

#define TASK_YIELD_WHILE(cond) this->state = __LINE__; case __LINE__: if ((cond)) return false;

#define TASK_ABORT() this->state = -1; return true;

#define SECOND *1000LL
#define SECONDS SECOND
#define MINUTE *(60LL*1000LL)
#define MINUTES MINUTE
#define HOUR *(3600LL*1000LL)
#define HOURS HOUR

#define TASK_SLEEP(timeout) this->sleep.Start(timeout); TASK_WAIT_FOR(&this->sleep);

#define TASK_PERIODICALLY(period, action) for (;;) {this->sleep.Start((period)); action; TASK_WAIT_FOR(&this->sleep);}

#define TASK_POLL(action) for(;;) {action; TASK_YIELD();}

#define TASK_WAIT_CONDITION(callback) TASK_WAIT_FOR((callback))

#define TASK_WAIT_SIGNAL(hSignal) TASK_WAIT_FOR((hSignal))

#define TASK_SET_SIGNAL(hSignal) (hSignal)->Set()

#define TASK_WAIT_VALUE(hValueHolder, variable) TASK_WAIT_FOR((hValueHolder));  variable = (hValueHolder)->Get();

#define TASK_SET_VALUE(hValueHolder, value) hValueHolder->Set(value);

#endif