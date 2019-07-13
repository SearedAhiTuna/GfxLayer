
#pragma once

#include "Types.h"
#include "TParabola.h"

#include <iostream>

class Parabola3D : public Function<vec3, GLfloat>
{
public:
    Parabola3D(const GLfloat& w, const GLfloat& h, const vec3& center_ = VEC3_ORIGIN, const vec3& raxis_ = VEC3_ORIGIN):
        base(w,h),
        center(center_),
        raxis(raxis_)
    {
    }

    vec3 operator()(const GLfloat& theta)
    {
        vec4 v(base(theta), 0, 1);

        if (raxis != VEC3_ORIGIN)
            v = rotate(length(raxis), normalize(raxis)) * v;
        v = translate(center) * v;

        return v.xyz;
    }

    GLfloat inv(const vec3& v, const unsigned int index = 0)
    {
        vec4 v4(v, 1);

        // Strip transformations
        v4 = translate(-center) * v4;
        if (raxis != VEC3_ORIGIN)
            v4 = rotate(-length(raxis), normalize(raxis)) * v4;

        // Feed result to base inverse
        vec2 v2 = v4.xy;
        return base.inv(v2);
    }

private:
    TParabola base;
    vec3 center;
    vec3 raxis;
};