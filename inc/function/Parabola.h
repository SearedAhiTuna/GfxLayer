
#pragma once

#include "Function.h"
#include "Types.h"
#include "Polynomial.h"

class Parabola : public Function<GLfloat, GLfloat>
{
public:
    Parabola(const GLfloat& w, GLfloat h):
        _w(w),
        _h(h)
    {
        base.resize(3);
        update();
    }

    GLfloat operator()(const GLfloat& x)
    {
        return base(x);
    }

    GLfloat inv(const GLfloat& y, const unsigned int index = 0)
    {
        return base.inv(y, index);
    }

    Parabola& w(const GLfloat& w)
    {
        _w = w;
        update();
    }

    const GLfloat& w() const
    {
        return _w;
    }

    Parabola& h(const GLfloat& h)
    {
        _h = h;
        update();
    }

    const GLfloat& h() const
    {
        return _h;
    }

    const GLfloat& a()
    {
        return base[2];
    }

    const GLfloat& c()
    {
        return base[0];
    }

private:
    void update()
    {
        base[0] = _h;
        base[2] = -4 * _h / powf(_w, 2);
    }

private:
    Polynomial base;
    GLfloat _w;
    GLfloat _h;
};