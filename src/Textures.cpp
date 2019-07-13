
#include "Textures.h"
#include "Graphics.h"
#include "InternalLibs.h"
#include "Types.h"

//#include <iostream>

#include "lodepng.h"

static GLuint loadPNG(const std::string& fn, const bool& alpha)
{
    LodePNGColorType colorType = alpha ? LCT_RGBA : LCT_RGB;

    std::vector<unsigned char> imgData;
    unsigned int w, h;

    lodepng::decode(imgData, w, h, fn, colorType, 8);

    const unsigned char* dataArr = imgData.data();

    uvec2 dims(w, h);
    return Textures::generate(dataArr, dims, alpha ? GL_RGBA : GL_RGB);
}

static TextureList textures;

Texture Textures::load(const std::string& fn, const bool& alpha)
{
    return textures.get(fn, alpha);
}

unsigned int Textures::generate(const void* data, const uvec2& dims, const GLenum& format, const GLenum& type)
{
    GLuint textureID;
    GRAPHICS_CALL_BEGIN(&textureID, format, dims, type, data)
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, w, h); GL_CHECK
        //glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, imgData.data()); GL_CHECK

        glTexImage2D(GL_TEXTURE_2D, 0, format, dims.x, dims.y, 0, format, type, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GRAPHICS_CALL_END

    return textureID;
}

void Textures::free(const GLuint& id)
{
    GRAPHICS_CALL_BEGIN(&id)
        glDeleteTextures(1, &id);
    GRAPHICS_CALL_END
}

Image TextureList::create(const std::string& fn, const bool& alpha)
{
    Image tex;

    std::string ext = fn.substr(fn.find_last_of('.'));

    if (ext == ".png")
    {
        tex.id = loadPNG(fn, alpha);
    }
    else
    {
        throwGraphicsError("Unrecognized image file format: " + ext);
    }

    //std::cout << "Created texture: " << tex.id << "\n";
    return tex;
}

void TextureList::destroy(const std::string& fn, Image& tex)
{
    Textures::free(tex.id);
    //std::cout << "Destroyed texture: " << tex.id << "\n";
}