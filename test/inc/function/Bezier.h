
#pragma once

#include <cmath>
#include <initializer_list>
#include <map>
#include <utility>
#include <vector>

#include "Function.h"
#include "Libs.h"

template <typename vec>
class Bezier : public Function<vec, GLfloat>, public std::vector<vec>
{
public:
    Bezier()
    {
    }

    Bezier(const std::initializer_list<vec>& vs)
    {
        std::vector<vec>::reserve(vs.size());

        for (const vec& v : vs)
            std::vector<vec>::push_back(v);
    }

    vec operator()(const GLfloat& t)
    {
        vec sum;

        size_t n = degree();
        for (size_t i = 0; i <= n; ++i)
        {
            const vec& v = std::vector<vec>::at(i);
            sum += bin_coeff(n, i) * powf(1 - t, (GLfloat)(n - i)) * pow(t, (GLfloat)i) * v;
        }

        return sum;
    }

    size_t degree()
    {
        return std::vector<vec>::size() - 1;
    }

private:
    typedef std::pair<size_t, size_t> nk;

    static size_t bin_coeff(const size_t& n, const size_t& k)
    {
        static std::map<nk, size_t> vals;
        
        if (k == 0 || n <= k)
            return 1;

        if (vals.count(nk(n, k)) != 0)
            return vals[nk(n, k)];

        size_t val = bin_coeff(n - 1, k - 1) + bin_coeff(n - 1, k);
        vals.emplace(nk(n, k), val);

        return val;
    }
};