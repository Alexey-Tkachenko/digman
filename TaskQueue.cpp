#include "TaskQueue.h"
#include "TaskBase.h"

#include "Assert.h"

TaskQueue::TaskQueue() : count()
{
}

bool TaskQueue::Add(TaskBase* task)
{
	if (task == nullptr)
	{
		return false;
	}
	if (this->count < this->capacity)
	{
		this->Tasks[this->count++] = task;
		return true;
	}
	return false;
}

bool TaskQueue::Remove(TaskBase* task)
{
	if (task == nullptr)
	{
		return false;
	}
	for (size_t i = 0; i < count; ++i)
	{
		if (task == this->Tasks[i])
		{
			for (size_t k = i + 1; k < count; ++k)
			{
				this->Tasks[k - 1] = this->Tasks[k];
			}
			this->Tasks[count - 1] = nullptr;
			--count;
			return true;
		}
	}
	return false;
}

size_t TaskQueue::Count() const
{
	return this->count;
}



TaskQueue::Iterator::Iterator(TaskQueue& queue) : queue(queue), position()
{
}

TaskQueue::Iterator& TaskQueue::Iterator::CurcularNext()
{
	if (++position >= queue.count)
	{
		position = 0;
	}
	return *this;
}

bool TaskQueue::Iterator::Next()
{
	return ++position < queue.count;
}

TaskBase* TaskQueue::Iterator::Current()
{
	if (position >= queue.count)
	{
		return nullptr;
	}
	return queue.Tasks[position];
}

TaskBase* TaskQueue::Iterator::operator->()
{
	return Current();
}

TaskQueue::Iterator::operator bool()
{
	return Current() != nullptr;
}
