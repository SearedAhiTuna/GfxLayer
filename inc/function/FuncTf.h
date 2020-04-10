
#pragma once

#include "Types.h"
#include "Function.h"

template <typename Out, typename In, typename Mat>
class FuncTf : public Function<Out, In>
{
private:
    typedef Function<Out, In> Func;

public:
    FuncTf(Func& f, Mat& m) :
        _f(&f),
        _ownsF(false),
        _m(m)
    {
    }

    FuncTf(Func* f, Mat& m) :
        _f(f),
        _ownsF(true),
        _m(m)
    {
    }

    ~FuncTf()
    {
        if (_ownsF && _f)
        {
            delete _f;
        }
    }

    Out operator()(const In& t)
    {
        // This definitely doesn't work with the templates but I don't care at this point
        vec4 input((*_f)(t), 1);
        return (_m * input).xyz;
    }

private:
    Func* _f;
    bool _ownsF;
    Mat _m;
};
