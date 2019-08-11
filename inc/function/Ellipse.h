
#pragma once

#include "Function.h"
#include "Types.h"

class Ellipse : public Function<vec2, GLfloat>
{
public:
    Ellipse(const GLfloat& w, const GLfloat& h, const GLfloat& x = 0, const GLfloat& y = 0) :
        _w(w),
        _h(h),
        _u(x),
        _v(y)
    {
    }

    vec2 operator()(const GLfloat& theta)
    {
        if (_u == 0 && _v == 0)
        {
            float x = _u + _w * cosf(theta);
            float y = _v + _h * sinf(theta);

            return vec2(x, y);
        }
            
        // I hate ellipses now
        float A = tanf(theta);
        float B = 2 * A * _v * _w;
        float C = -2 * A * A * _u * _w;
        float S = (cosf(theta) >= 0) ? 1.0f : -1.0f;
        float D = -4 * A * A * _u * _u;
        float E = 8 * A * _u * _v;
        float F = A * A * _w * _w;
        float G = _h * _h;
        float H = -4 * _v * _v;
        float I = A * A * _w * _w;
        float J = _h * _h;

        float x = _w * (B + C + S * _h * sqrtf(D + E + F + G + H)) / (2 * (I + J)) + _u;
        float y = A * x;

        return vec2(x, y);
    }

    GLfloat inv(const vec2& v, const unsigned int index = 0)
    {
        return atan2f(v.y, v.x);
    }

    static Ellipse construct_fit(const GLfloat& w, const GLfloat& h, const GLfloat& r, const GLfloat& theta)
    {

    }

private:
    GLfloat _w;
    GLfloat _h;
    GLfloat _u;
    GLfloat _v;
};