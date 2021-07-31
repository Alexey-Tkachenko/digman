#ifndef _MEDIAN_H_
#define _MEDIAN_H_

template<typename T, int N, typename IDX = int>
class Median
{
	T values[N];
	mutable T tmp[N];
	IDX i;

public:
	Median() : values(), i()
	{
	}

	void write(T val)
	{
		values[i++] = val;
		if (i == N) i = 0;
	}

	T get() const
	{
		for (IDX x = 0; x != N; ++x)
		{
			tmp[x] = values[x];
		}
		for (IDX a = 0; a != (N - 1); ++a)
		{
			for (IDX b = a + 1; b != N; ++b)
			{
				if (tmp[b] < tmp[a])
				{
					T q = tmp[b];
					tmp[b] = tmp[a];
					tmp[a] = q;
				}
			}
		}
		return tmp[N >> 1];
	}

    void fill(int saved)
	{
        for (IDX i = 0; i < N; ++i)
        {
            values[i] = saved;
        }
	}
};

template<typename T, typename IDX>
class Median<T, 1, IDX>
{
	T val;
public:
	Median() : val() {}
	void write(T val)
	{
		this->val = val;
	}

	T get() const
	{
		return val;
	}

    void fill(T saved)
	{
	    val = saved;
	}
};


#endif

