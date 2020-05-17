
#pragma once

#ifdef BUILD_GFXLAYER

#include <GL/glew.h>

#else

#include "FakeGlew.h"

#endif

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#define VEC3_ORIGIN vec3(0, 0, 0)
#define VEC3_UP vec3(0, 1, 0)
#define VEC3_DOWN vec3(0, -1, 0)
#define VEC3_LEFT vec3(-1, 0, 0)
#define VEC3_RIGHT vec3(1, 0, 0)
#define VEC3_FORWARDS vec3(0, 0, -1)
#define VEC3_BACKWARDS vec3(0, 0, 1)
#define VEC3_TOWARDS_ME vec3(0, 0, 1)
#define VEC3_INTO_SCREEN vec3(0, 0, -1)

#define VEC3_X_AXIS vec3(1, 0, 0)
#define VEC3_Y_AXIS vec3(0, 1, 0)
#define VEC3_Z_AXIS vec3(0, 0, 1)

#define VEC3_IDENT vec3(1, 1, 1)

#define UV_TOP_LEFT vec2(0, 0)
#define UV_BOTTOM_LEFT vec2(0, 1)
#define UV_TOP_RIGHT vec2(1, 0)
#define UV_BOTTOM_RIGHT vec2(1, 1)

#define VEC2_ORIGIN vec2(0, 0)
#define VEC2_UP vec2(0, 1)
#define VEC2_DOWN vec2(0, -1)
#define VEC2_LEFT vec2(-1, 0)
#define VEC2_RIGHT vec2(1, 0)

#define MAT3_IDENT mat3()
#define MAT4_IDENT mat4()

extern const GLfloat PI;

struct GLFWwindow;

#include <iostream>

inline std::ostream& operator<<(std::ostream& out, const vec2& vec)
{
    return out << "(" << vec.x << "," << vec.y << ")";
}

inline std::ostream& operator<<(std::ostream& out, const vec3& vec)
{
    return out << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}

inline std::ostream& operator<<(std::ostream& out, const vec4& vec)
{
    return out << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}

namespace glm
{
    inline mat3 scale3(const vec3& v)
    {
        mat3 s;
        s[0][0] = v.x;
        s[1][1] = v.y;
        s[2][2] = v.z;
        return s;
    }
}