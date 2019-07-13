
#pragma once

#include "Libs.h"

#include <vector>

namespace std { class any; }

class vecx
{
public:
    template <typename vec>
    vecx(const vec& v);

    size_t length() const;

    GLfloat& operator[](const size_t& ind);
    const GLfloat& operator[](const size_t& ind) const;

private:
    std::vector<GLfloat> values;
};

template <typename vec>
vecx::vecx(const vec& v)
{
    values.reserve(v.length());
    for (size_t i = 0; i < v.length(); ++i)
    {
        values.emplace_back(v[i]);
    }
}

