
#pragma once

#include <cmath>
#include <stdexcept>
#include <vector>

#include "Function.h"
#include "Libs.h"

class Polynomial : public Function<GLfloat, GLfloat>, public std::vector<GLfloat>
{
public:
    GLfloat operator()(const GLfloat& x)
    {
        GLfloat sum = 0;

        for (size_t n = 0; n < size(); ++n)
        {
            sum += at(n) * powf(x, n);
        }

        return sum;
    }

    GLfloat inv(const GLfloat& y, const unsigned int index = 0)
    {
#define a at(2)
#define b at(1)
#define c at(0)
        if (degree() == 0)
        {
            if (y == c)
                return y;
            else
                throw std::domain_error(__PRETTY_FUNCTION__ ": Argument out of range");
        }
        else if (degree() == 1)
        {
            return (y - c) / b;
        }
        else if (degree() == 2)
        {
            GLfloat arg = powf(b, 2) - 4 * a * c + 4 * a * y;

            if (arg < 0)
                throw std::domain_error(__PRETTY_FUNCTION__ ": Argument out of range");

            if (index % 2 == 0)
                return (-b - sqrtf(arg)) / (2 * a);
            else
                return (-b + sqrtf(arg)) / (2 * a);
        }
        else
        {
            throw std::invalid_argument(__PRETTY_FUNCTION__ ": Not defined for higher degrees");
        }
#undef a
#undef b
#undef c
    }

    size_t degree()
    {
        return size() - 1;
    }
};