
#pragma once

#include "Function.h"
#include "Types.h"
#include "Parabola.h"

#include <iostream>

// Parabola positions:
// theta = 0 -> (w/2, 0)
// theta = PI/2 -> (0, h)
// theta = PI -> (-w/2, 0)

class TParabola : public Function<vec2, GLfloat>
{
public:
    TParabola(const GLfloat& w, const GLfloat& h) :
        base(w,h)
    {
    }

    vec2 operator()(const GLfloat& theta)
    {
        if (cosf(theta) == 0)
            return vec2(0, base.h());

        const GLfloat& a = base.a();
        const GLfloat& c = base.c();

        const GLfloat T = tanf(theta);
        const int s = cosf(theta) >= 0 ? 1 : -1;

        const GLfloat B = -s * T;

        const GLfloat x = s * (-B - sqrtf(powf(B, 2) - 4 * a * c)) / (2 * a);

        return vec2(x, base(x));
    }

    GLfloat inv(const vec2& v, const unsigned int index = 0)
    {
        return atan2f(v.y, v.x);
    }

public:
    Parabola base;
};