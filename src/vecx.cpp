
#include "vecx.h"

#include <any>

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