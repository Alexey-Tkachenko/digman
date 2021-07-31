#include "Scheduler.h"

Scheduler::Scheduler() 
: realTimeTasks()
, normalTasks()
,normalTasksIterator(normalTasks)
#ifdef SCHEDULER_SUPPORTS_IDLE
, idleTasks()
, idleTasksIterator(idleTasks)
#endif
{
}

bool Scheduler::Register(TaskBase& task, TaskPriority priority)
{
	switch (priority)
	{
	case TaskPriority::SensorPoll:
		return sensorPollerTasks.Add(&task);
	case TaskPriority::RealTime:
		return realTimeTasks.Add(&task);
	case TaskPriority::Normal:
		return normalTasks.Add(&task);
#ifdef SCHEDULER_SUPPORTS_IDLE
	case TaskPriority::Idle:
		return idleTasks.Add(&task);
#endif
	}
	return false;
}

void Scheduler::PollSensors()
{
	TaskQueue::Iterator poller = sensorPollerTasks;
	while (poller)
	{
		if (poller->IsWaiting())
		{
			if (!poller.Next())
			{
				break;
			}
		}
		else
		{
			if (poller->StepInternal())
			{
				sensorPollerTasks.Remove(poller.Current());
			}
			else
			{
				if (!poller.Next())
				{
					break;
				}
			}
		}
	}
}

bool Scheduler::RealTime()
{
	bool invoked = false;
	TaskQueue::Iterator realTimeIterator = realTimeTasks;
	while (realTimeIterator)
	{
		if (!realTimeIterator->IsWaiting())
		{
			invoked = true;
			if (realTimeIterator->StepInternal())
			{
				realTimeTasks.Remove(realTimeIterator.Current());
				continue;
			}
		}
		realTimeIterator.Next();
	}
	return invoked;
}

bool Scheduler::TickNormal(unsigned long expiration)
{
	bool invoked = false;
	size_t oldPosition = normalTasksIterator.Position();
	do
	{
		if (normalTasksIterator && !normalTasksIterator->IsWaiting())
		{
			if (normalTasksIterator->StepInternal())
			{
				if (normalTasksIterator.Position() < oldPosition)
				{
					--oldPosition;
				}
				normalTasks.Remove(normalTasksIterator.Current());
			}
			invoked = true;
		}
		normalTasksIterator.CurcularNext();
	} while (oldPosition != normalTasksIterator.Position() && (millis() < expiration));
	return invoked;
}

#ifdef SCHEDULER_SUPPORTS_IDLE
bool Scheduler::TickIdle(unsigned long expiration)
{
	size_t oldPosition = idleTasksIterator.Position();
	do
	{
		if (idleTasksIterator && !idleTasksIterator->IsWaiting())
		{
			if (expiration < millis())
			{
				return false;
			}
			if (idleTasksIterator->StepInternal())
			{
				idleTasks.Remove(idleTasksIterator.Current());
				return true;
			}
			else
			{
				idleTasksIterator.CurcularNext();
				return true;
			}
		}
	} while (idleTasksIterator && idleTasksIterator.Position() != oldPosition);
	return false;
}
#endif

void Scheduler::Invoke()
{
	PollSensors();
	unsigned long expiration = millis() + Period;
    #ifdef SCHEDULER_SUPPORTS_IDLE
    bool rt = 
    #endif
    RealTime();
	TickNormal(expiration);
#ifdef SCHEDULER_SUPPORTS_IDLE	
	if (rt == false && millis() < expiration)
	{
		TickIdle(expiration);
	}
#endif

#ifdef SHEDULER_WAIT_REST
	if (millis() < expiration)
	{
		if (!RealTime())
		{
			int rest = expiration - millis();
			if (rest > 0)
			{
				delay(rest);
			}
		}
	}
#endif

}

void Scheduler::PrintStatistics(Stream& p)
{
	p.print(F("Sensor poll: ")); p.println(this->sensorPollerTasks.Count());
	p.print(F("Real-time:   ")); p.println(this->realTimeTasks.Count());
	p.print(F("Normal:      ")); p.println(this->normalTasks.Count());
#ifdef SCHEDULER_SUPPORTS_IDLE
	p.print(F("Idle:        ")); p.println(this->idleTasks.Count());
#endif
}
