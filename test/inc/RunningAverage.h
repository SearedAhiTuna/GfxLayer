
#pragma once

#include <list>

template <typename T>
class RunningAverage
{
public:
    RunningAverage(const size_t& depth = 1):
        _depth(depth)
    {
    }

    RunningAverage& enter(const T& t)
    {
        _values.push_back(t);
        if (_values.size() > _depth)
            _values.pop_front();

        return *this;
    }

    T get()
    {
        T sum = T();
        for (const T& t : _values)
        {
            sum += t;
        }
        return sum / _values.size();
    }

private:
    size_t _depth;
    std::list<T> _values;
};