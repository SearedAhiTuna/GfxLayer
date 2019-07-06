
#pragma once

#include "Libs.h"

#include <memory>
#include <string>

struct FontInfo;

struct Letter
{
    GLuint tex;
    uvec2 dims;
};

class Font
{
public:
    Font(const std::string& fn, const size_t& size, const vec3& color = VEC3_ORIGIN);

    Letter get(const wchar_t& c);
    Letter get(const char& c);

private:
    std::shared_ptr<FontInfo> _info;

    size_t _size;
    vec3 _color;
};
