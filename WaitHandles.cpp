#include "Arduino.h"
#include "WaitHandles.h"
#include "PlacementNew.h"

namespace WaitHandles
{

	Sleep::Sleep() : expired()
	{}

	Sleep::Sleep(unsigned interval) : expired(millis() + interval)
	{}

	void Sleep::Start(unsigned interval)
	{
		this->expired = millis() + interval;
	}

	bool Sleep::IsSignalled()
	{
		return millis() >= expired;
	}

	Event::Event() : active(false)
	{}

	Event::Event(bool active) : active(active)
	{}

	bool Event::IsSignalled()
	{
		return active;
	}

	void Event::Set()
	{
		active = true;
	}

	void Event::Reset()
	{
		active = false;
	}

	AutoResetEvent::AutoResetEvent() : Event()
	{}

	AutoResetEvent::AutoResetEvent(bool active) : Event(active)
	{}


	bool AutoResetEvent::IsSignalled()
	{
		if (Event::IsSignalled())
		{
			Reset();
			return true;
		}
		return false;
	}


	Condition::Condition()
		: condition()
	{}

	Condition::Condition(WaitCondition* condition) : condition(condition)
	{}

	bool Condition::IsSignalled()
	{
		return (condition == nullptr) || (*condition)();
	}

	Condition::Condition(bool(*arg)())
	{
		SetCondition(arg);
	}


	void Condition::SetCondition(WaitCondition* condition)
	{
		this->condition = condition;
	}

	void Condition::SetCondition(bool(*arg)())
	{
		this->condition = new(this->ExtWaitCondition)FuncWaitCondition(arg);
	}


	FuncWaitCondition::FuncWaitCondition(bool(*arg)())
		: arg(arg)
	{
	}

	bool FuncWaitCondition::operator()()
	{
		return arg();
	}

	FuncWaitCondition Wrap(bool(*arg)())
	{
		return FuncWaitCondition(arg);
	}

}