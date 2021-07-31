#ifndef _TASK_BASE_
#define _TASK_BASE_

#include "Scheduler.h"
#include "WaitObject.h"
#include "WaitHandles.h"

class TaskQueue;

class TaskBase
{
protected:
	TaskBase();
	virtual bool Step() = 0;

	void WaitFor(WaitHandle wait);

private:
	WaitHandle wait;
	
	friend class TaskQueue;
	friend class Scheduler;

	bool IsWaiting();
	
	TaskBase& operator=(const TaskBase&) = delete;
	TaskBase(const TaskBase&) = delete;
	bool StepInternal();

};

class StatefullTaskBase : public TaskBase
{
protected:
	int state;
	StatefullTaskBase() : state(0){}
	void XReset()
	{
		state = 0;
	}

	WaitHandles::Sleep sleep;
private:
	StatefullTaskBase& operator=(const StatefullTaskBase&) = delete;
	StatefullTaskBase(const StatefullTaskBase&) = delete;
	
};


#endif