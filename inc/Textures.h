
#pragma once

#include "Types.h"
#include "RefCntDict.h"

#include <string>

struct Image
{
    GLuint id{};

    bool valid() { return id != 0; }
};

class TextureList : public RefCntDict<std::string, Image, const bool&>
{
private:
    Image create(const std::string& fn, const bool& alpha);
    void destroy(const std::string& fn, Image& tex);
};

typedef TextureList::Ref Texture;

namespace Textures
{
    Texture load(const std::string& fn, const bool& alpha = false);

    unsigned int generate(const void* data, const uvec2& dims, const GLenum& format, const GLenum& type = GL_UNSIGNED_BYTE);
    void free(const GLuint& id);
}