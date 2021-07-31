#include "TaskBase.h"

TaskBase::TaskBase() : wait()
{}

bool TaskBase::StepInternal()
{
	if (this->Step())
	{
		return true;
	}
	return false;
}

void TaskBase::WaitFor(WaitHandle handle)
{
	this->wait = handle;
}

bool TaskBase::IsWaiting()
{
	if (wait == nullptr)
	{
		return false;
	}
	return !wait->IsSignalled();
}
