#pragma once

template<typename T>
class Kalman
{
    double k;
    double s;
    bool empty;
public:
    explicit Kalman(double k) : k(k), s(), empty(true)
    {
    }

    T operator()(T input)
    {
        return step(input);
    }

    T step(T input)
    {
        if (empty)
        {
            empty = false;

            s = input;
            return input;
        }
        
        s = (1 - k) * s + k * input;

        return (T)(0.5 + s);
    }

    void fill(int saved)
    {
        s = saved;
    }
};
