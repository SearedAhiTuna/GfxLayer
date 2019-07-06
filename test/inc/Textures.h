
#pragma once

#include "RefCntDict.h"

#include <string>

struct Tex_int
{
    unsigned int id{};

    bool valid() { return id != 0; }
};

class TextureList : public RefCntDict<std::string, Tex_int, const bool&>
{
public:
    Tex_int create(const std::string& fn, const bool& alpha);
    void destroy(const std::string& fn, Tex_int& tex);
};

typedef TextureList::Ref Texture;

namespace Textures
{
    Texture load(const std::string& fn, const bool& alpha = false);

    unsigned int generate(const unsigned char* data, unsigned int& w, const unsigned int& h, const bool& alpha = false);
    void free(const unsigned int& id);
}