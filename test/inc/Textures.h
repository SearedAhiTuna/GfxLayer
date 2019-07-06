
#pragma once

#include <string>

namespace Textures
{
    unsigned int load(const std::string& fn, const bool& alpha = false);
    void free(const unsigned int& id);

    unsigned int generate(const unsigned char* data, unsigned int& w, const unsigned int& h, const bool& alpha = false);
}