
#include "Textures.h"
#include "Graphics.h"
#include "Libs.h"

#include "lodepng.h"
#include <unordered_map>

static std::mutex glTextureMutex;

struct TexInfo
{
    GLuint id;
    unsigned int refCount;

    TexInfo() :
        id(0),
        refCount(0)
    {
    }

    TexInfo(const GLuint& _id) :
        id(_id),
        refCount(1)
    {
    }
};

static std::unordered_map<std::string, TexInfo> texDB;
static std::unordered_map<GLuint, std::string> texNameDB;

static GLuint loadPNG(const std::string& fn, const bool& alpha)
{
    LodePNGColorType colorType = alpha ? LCT_RGBA : LCT_RGB;

    std::vector<unsigned char> imgData;
    unsigned int w, h;

    lodepng::decode(imgData, w, h, fn, colorType, 8);

    const unsigned char* dataArr = imgData.data();

    /*std::cout << "Decoded PNG file: " << w << "x" << h << std::endl;

    for (int i = 0; i < imgData.size() && i < 24; ++i)
    {
        std::cout << (int)imgData.data()[i] << " ";
    }
    std::cout << "\n";*/

    return Textures::generate(dataArr, w, h, alpha);
}

unsigned int Textures::load(const std::string& fn, const bool& alpha)
{
    std::lock_guard<std::mutex> lk(glTextureMutex);

    if (texDB.count(fn))
    {
        ++texDB[fn].refCount;
        return texDB[fn].id;
    }

    std::string ext = fn.substr(fn.find_last_of('.'));

    GLuint textureID = 0;

    if (ext == ".png")
    {
        textureID = loadPNG(fn, alpha);
    }
    else
    {
        throwGraphicsError("Unrecognized image file format: " + ext);
    }

    texDB.emplace(fn, textureID);
    texNameDB[textureID] = fn;
    return textureID;
}

void Textures::free(const unsigned int& id)
{
    std::lock_guard<std::mutex> lk(glTextureMutex);

    if (texNameDB.count(id) == 0)
    {
        throwGraphicsError("Attempted to free uninitialized texture: " + id);
    }

    const std::string& fn = texNameDB[id];
    TexInfo& info = texDB[fn];

    if (--info.refCount == 0)
    {
        texDB.erase(fn);
        texNameDB.erase(id);

        GRAPHICS_CALL_BEGIN(&id)
            glDeleteTextures(1, &id);
        GRAPHICS_CALL_END
    }
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