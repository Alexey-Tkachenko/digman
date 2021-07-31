#ifndef _WAIT_HANDLES_
#define _WAIT_HANDLES_

#include "Arduino.h"
#include "WaitObject.h"
#include "ConditionWrapper.h"
#include "PlacementNew.h"
#include "Assert.h"

#define DELETE_COPY(T) T(const T&) = delete; T& operator=(const T&) = delete;

namespace WaitHandles
{

	class Sleep : public WaitObject
	{
	public:
		Sleep();
		Sleep(unsigned interval);
		DELETE_COPY(Sleep);
		void Start(unsigned interval);
		virtual bool IsSignalled() override;
	private:
		unsigned long expired;
	};

	class Event : public WaitObject
	{
	public:
		Event();
		Event(bool active);
		DELETE_COPY(Event);

		virtual bool IsSignalled() override;

		void Set();
		void Reset();

	private:
		volatile bool active;
	};


	class AutoResetEvent : public Event
	{
	public:
		AutoResetEvent();
		AutoResetEvent(bool active);
		DELETE_COPY(AutoResetEvent);
		virtual bool IsSignalled() override;
	};



	class Condition : public WaitObject
	{
	public:
		Condition();

        DELETE_COPY(Condition);

		template<class T>
		Condition(bool(*arg)(T*), T* data)
		{
			SetCondition(arg, data);
		}

		template<class T>
		Condition(T* target, bool(T::*method)())
		{
			SetCondition(target, method);
		}

		Condition(bool(*arg)());

		Condition(WaitCondition* condition);

		void SetCondition(WaitCondition* condition);

		void SetCondition(bool(*arg)());

		template<class T>
		void SetCondition(bool(*arg)(T*), T* data)
		{
			condition = new(ExtWaitConditionT)TFuncWaitCondition<T>(arg, data);
		}

		template<class T>
		void SetCondition(T* target, bool(T::*method)())
		{
			condition = new (ExtMemberCondition)MemberFuncWaitCondition<T>(target, method);
		}

		virtual bool IsSignalled() override;

	private:
		WaitCondition* condition;

		struct Dummy{};

		union
		{
			byte ExtWaitCondition[sizeof(FuncWaitCondition)];
			byte ExtWaitConditionT[sizeof(TFuncWaitCondition<void*>)];
			byte ExtMemberCondition[sizeof(MemberFuncWaitCondition<Dummy>)];
		};
	};

	template<typename T>
	class ValueHolder : public WaitObject
	{
	public:
		ValueHolder() : value(), signalled(false) {}
		ValueHolder(const T& initial) : value(initial), signalled(false) 
		{}

        DELETE_COPY(ValueHolder);

		const T& Get()
		{
			signalled = false;
			return this->value;
		}

		void Set(const T& value)
		{
			this->value = value;
			signalled = true;
		}

		virtual bool IsSignalled() override
		{
			return signalled;
		}

	private:
		T value;
		volatile bool signalled;
	};

    template<typename T>
    struct Empty
    {
        static const T& Value()
        {
            static T value{};
            return value;
        }
    };

    template<typename T, size_t Capacity, typename SizeType = size_t>
    class DataQueue : public WaitObject
    {
    private:
        static SizeType Next(SizeType& pos)
        {
            ++pos;
            if (pos == Capacity)
            {
                pos = 0;
            }
            return pos;
        }
    public:
        DataQueue() : values() {}

        DELETE_COPY(DataQueue);

        const T& Get()
        {
            if (size == 0) return Empty<T>::Value();

            T& value = values[read];
            Next(read);
            --size;
            return value;
        }

        bool Put(const T& value)
        {
            if (size == Capacity)
            {
                return false;
            }

            values[write] = value;
            Next(write);
            ++size;
            return true;
        }

        virtual bool IsSignalled() override
        {
            return size != 0;
        }

        SizeType Size() const
        {
            return size;
        }

    private:
        T values[Capacity];
        SizeType read;
        SizeType write;
        SizeType size;
    };
}


#undef DELETE_COPY
#endif
