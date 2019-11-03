
#pragma once

#include "Types.h"
#include "Function.h"

template <typename Out, typename In>
class FuncSum : public Function<Out, In>
{
private:
    typedef Function<Out, In> Func;

public:
    FuncSum(Func& f1, Func& f2, GLfloat weight):
        _f1(f1),
        _f2(f2),
        _weight(weight)
    {
    }

    Out operator()(const In& t)
    {
        return _f1(t) * (1 - _weight) + _f2(t) * _weight;
    }

private:
    Func& _f1;
    Func& _f2;
    GLfloat _weight;
};
