
#pragma once

#include "Types.h"
#include "Ellipse.h"

class Ellipse3D : public Function<vec3, GLfloat>
{
public:
    Ellipse3D(const Ellipse& base, const vec3& center = vec3(0, 0, 0), const vec3& xaxis = vec3(1, 0, 0), const vec3& yaxis = vec3(0, 1, 0)) :
        _base(base),
        _center(center),
        _xaxis(normalize(xaxis)),
        _yaxis(normalize(yaxis))
    {
    }

    vec3 operator()(const GLfloat& theta)
    {
        vec2 v = _base(theta);

        vec3 xcomp = v.x * _xaxis;
        vec3 ycomp = v.y * _yaxis;

        return xcomp + ycomp + _center;
    }

private:
    Ellipse _base;
    vec3 _center;
    vec3 _xaxis;
    vec3 _yaxis;
};