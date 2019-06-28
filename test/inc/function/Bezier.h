
#pragma once

#include <cmath>
#include <map>
#include <vector>

#include "Function.h"
#include "Libs.h"

template <typename vec>
class Bezier : public Function<vec, GLfloat>, public std::vector<vec>
{
public:
    vec operator()(const GLfloat& t)
    {
        vec sum;

        size_t n = degree();
        for (size_t i = 0; i <= n; ++i)
        {
            sum += bin_coeff(n, i) * powf(1 - t, n - i) * pow(t, i) * at(i);
        }

        return sum;
    }

    size_t degree()
    {
        return size() - 1;
    }

private:
    struct nk
    {
        size_t n;
        size_t k;

        nk(const size_t& _n = 0, const size_t& _k = 0) :
            n(_n),
            k(_k)
        {
        }
    };

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