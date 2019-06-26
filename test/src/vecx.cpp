
#include "vecx.h"

#include <any>

vecx::vecx(const std::any a)
{
    try
    {
        vec2 v = std::any_cast<vec2>(a);
        values.reserve(v.length());
        for (size_t i = 0; i < v.length(); ++i)
        {
            values.emplace_back(v[i]);
        }
    }
    catch (const std::bad_any_cast&)
    {
    }

    try
    {
        vec3 v = std::any_cast<vec3>(a);
        values.reserve(v.length());
        for (size_t i = 0; i < v.length(); ++i)
        {
            values.emplace_back(v[i]);
        }
    }
    catch (const std::bad_any_cast&)
    {
    }

    try
    {
        vec4 v = std::any_cast<vec4>(a);
        values.reserve(v.length());
        for (size_t i = 0; i < v.length(); ++i)
        {
            values.emplace_back(v[i]);
        }
    }
    catch (const std::bad_any_cast&)
    {
    }
}

size_t vecx::length() const
{
    return values.size();
}

GLfloat& vecx::operator[](const size_t& ind)
{
    return values[ind];
}

const GLfloat& vecx::operator[](const size_t& ind) const
{
    return values[ind];
}