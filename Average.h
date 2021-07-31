#pragma once

#ifndef _AVERAGE_H_
#define _AVERAGE_H_

template<typename T, int N, typename IDX = int, typename SUM = T>
class Average
{
    T values[N];
    IDX i;
    IDX count;
    SUM sum;

public:
    Average() : values(), i()
    {
    }

    void write(T val)
    {
        if (count < N)
        {
            count += 1;
        }
        else
        {
            sum -= values[i];
        }
        sum += val;

        values[i++] = val;
        if (i == N) i = 0;
    }

    T get() const
    {
        if (count == 0) return T{};
        return T{sum / count};
    }

    void fill(T saved)
    {
        for (IDX i = 0; i < N; ++i)
        {
            values[i] = saved;
        }
    }
};

template<typename T, typename IDX>
class Average<T, 1, IDX, T>
{
    T current = T{};
public:
    void write(T val)
    {
        current = val;
    }

    T get() const
    {
        return current;
    }

    void fill(T saved)
    {
        current = saved;
    }
};

#endif
