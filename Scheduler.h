#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "TaskBase.h"
#include "TaskQueue.h"

enum class TaskPriority
{
#ifdef SCHEDULER_SUPPORTS_IDLE
	Idle = 0,
#endif
	Normal = 1,
	RealTime = 2,
	SensorPoll = 3,
};

class Scheduler
{
public:
	Scheduler();

	bool Register(TaskBase& task, TaskPriority priority = TaskPriority::Normal);

	void Invoke();
	
	void PrintStatistics(Stream& output);

private:
	void PollSensors();

	bool RealTime();

	bool TickNormal(unsigned long expiration);

#ifdef SCHEDULER_SUPPORTS_IDLE
	bool TickIdle(unsigned long expiration);
#endif

private:
	TaskQueue sensorPollerTasks;
	TaskQueue realTimeTasks;

	TaskQueue normalTasks;
	TaskQueue::Iterator normalTasksIterator;
	
#ifdef SCHEDULER_SUPPORTS_IDLE
	TaskQueue idleTasks;
	TaskQueue::Iterator idleTasksIterator;
#endif
#ifndef SCHEDULER_PERIOD
	static const int Period = 100;	
#else 
	static const int Period = SCHEDULER_PERIOD;
#endif
};

struct TaskPriorityPair
{
	TaskBase& Task;
	TaskPriority Priority;
};

inline Scheduler& operator << (Scheduler& scheduler, const TaskPriorityPair& pair)
{
	scheduler.Register(pair.Task, pair.Priority);
	return scheduler;
}


#endif
