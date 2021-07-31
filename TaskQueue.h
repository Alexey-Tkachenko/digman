#ifndef _TASK_QUEUE_
#define _TASK_QUEUE_

#include "Arduino.h"

#ifndef TASK_QUEUE_CAPACITY
#define TASK_QUEUE_CAPACITY 8
#endif


class TaskBase;

class TaskQueue
{
public:
	TaskQueue();

	bool Add(TaskBase* task);
	
	bool Remove(TaskBase* task);

	size_t Count() const;
private:
	TaskBase* Tasks[TASK_QUEUE_CAPACITY];
	size_t count;
	const size_t capacity = TASK_QUEUE_CAPACITY;

	friend struct TaskQueueIterator;
public:
	class Iterator
	{
	public:
		Iterator(TaskQueue& queue);
		Iterator& CurcularNext();
		bool Next();
		TaskBase* Current();
		TaskBase* operator->();
		operator bool();
		inline size_t Position() const
		{
			return position;
		}
	private:
		TaskQueue& queue;
		size_t position;
	};
};


#endif
