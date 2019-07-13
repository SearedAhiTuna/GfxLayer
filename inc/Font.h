
#pragma once

#include "Types.h"

#include <memory>
#include <string>

struct FontInfo;

struct Letter
{
    GLuint tex;
    uvec2 dims;
    unsigned int top;
};

class Font
{
public:
    Font(const std::string& fn, const size_t& size, const vec3& color = VEC3_ORIGIN);

    Letter get(const wchar_t& c);
    Letter get(const char& c);

    GLuint size() { return (GLuint)_size; }

    GLuint kerning(const wchar_t& c1, const wchar_t& c2);
    GLuint kerning(const char& c1, const char& c2);

private:
    std::shared_ptr<FontInfo> _info;

    size_t _size;
    vec3 _color;
};
