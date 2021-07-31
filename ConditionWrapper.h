#ifndef _CONDITION_WRAPPER_
#define _CONDITION_WRAPPER_

namespace WaitHandles
{
	struct WaitCondition
	{
		virtual bool operator()() = 0;
	};

	struct FuncWaitCondition : public WaitCondition
	{
		FuncWaitCondition(const FuncWaitCondition& src);
		FuncWaitCondition(bool(*arg)());
		virtual bool operator()() override;
	private:
		bool(*arg)();
	};

	template<class T>
	struct TFuncWaitCondition : public WaitCondition
	{
		TFuncWaitCondition(bool(*arg)(T*), T* data) : arg(arg), data(data){}
		virtual bool operator()() override{
			return arg(data);
		}
	private:
		bool(*arg)(T*);
		T* data;
	};

	FuncWaitCondition Wrap(bool(*arg)());

	template<class T>
	TFuncWaitCondition<T> Wrap(bool(*arg)(T*), T* data)
	{
		return TFuncWaitCondition<T>(arg, data);
	}

	template<class T>
	struct MemberFuncWaitCondition : public WaitCondition
	{
		MemberFuncWaitCondition(T* target, bool(T::*method)()) : target(target), method(method){}


		virtual bool operator()() override
		{
			return (target->*method)();
		}

	private:
		T* target;
		bool(T::*method)();
	};

	template<class T>
	MemberFuncWaitCondition<T> Wrap(T* target, bool(T::*method)())
	{
		return MemberFuncWaitCondition<T>(target, method);
	}

}
#endif
