#ifndef _MAYBE_
#define _MAYBE_

template<class T>
struct Maybe
{
	bool HasValue;
	T Value;

	Maybe(T value) : HasValue(true), Value(value){}
	explicit Maybe() : HasValue(false), Value(){}

	operator bool() const
	{
		return HasValue;
	}
};

template<class T>
const Maybe<T> None()
{
	return Maybe<T>();
}

#endif
