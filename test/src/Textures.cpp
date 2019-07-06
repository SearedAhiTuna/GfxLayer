
#include "Textures.h"
#include "Graphics.h"
#include "Libs.h"

//#include <iostream>
#include "lodepng.h"

static GLuint loadPNG(const std::string& fn, const bool& alpha)
{
    LodePNGColorType colorType = alpha ? LCT_RGBA : LCT_RGB;

    std::vector<unsigned char> imgData;
    unsigned int w, h;

    lodepng::decode(imgData, w, h, fn, colorType, 8);

    const unsigned char* dataArr = imgData.data();

    return Textures::generate(dataArr, w, h, alpha);
}

static TextureList textures;

Texture Textures::load(const std::string& fn, const bool& alpha)
{
    return textures.get(fn, alpha);
}

unsigned int Textures::generate(const unsigned char* data, unsigned int& w, const unsigned int& h, const bool& alpha)
{
    GLuint textureID;
    GRAPHICS_CALL_BEGIN(&textureID, alpha, w, h, data)
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, w, h); GL_CHECK
        //glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, imgData.data()); GL_CHECK

        glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, w, h, 0,
            alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GRAPHICS_CALL_END

    return textureID;
}

void Textures::free(const unsigned int& id)
{
    GRAPHICS_CALL_BEGIN(&id)
        glDeleteTextures(1, &id);
    GRAPHICS_CALL_END
}

Tex_int TextureList::create(const std::string& fn, const bool& alpha)
{
    Tex_int tex;

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

void TextureList::destroy(const std::string& fn, Tex_int& tex)
{
    Textures::free(tex.id);
    //std::cout << "Destroyed texture: " << tex.id << "\n";
}