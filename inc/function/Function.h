
#pragma once

#include <stdexcept>

template <typename Out, typename In>
class Function
{
public:
    virtual Out operator()(const In&) = 0;
    virtual In inv(const Out&, const unsigned int index = 0)
    {
        throw std::runtime_error("Inverse not defined");
    }
};
