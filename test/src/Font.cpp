
#include "Font.h"
#include "Graphics.h"
#include "Textures.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstring>
#include <iostream>
#include <iomanip>

static FT_Library ftLib;
static bool _ftInit = false;

static void ftInit()
{
    auto err = FT_Init_FreeType(&ftLib);
    if (err)
    {
        throwGraphicsError("Failed to initialize FreeType");
    }
    _ftInit = true;
}

struct FontInfo
{
    FT_Face face{};
    bool valid{};

    FontInfo(const std::string& fn, const size_t& size)
    {
        ftInit();

        auto err = FT_New_Face(ftLib, fn.c_str(), 0, &face);
        if (err == FT_Err_Unknown_File_Format)
        {
            std::cerr << "[FreeType] Unknown font file format: " << fn << std::endl;
            return;
        }
        else if (err)
        {
            std::cerr << "[FreeType] Failed to load font: " << fn << std::endl;
            return;
        }

        err = FT_Set_Pixel_Sizes(face, 0, (FT_UInt)size);
        if (err)
        {
            std::cerr << "[FreeType] Failed to set font size: "
                      << fn << ", size=" << size << std::endl;
            return;
        }

        valid = true;
    }

    ~FontInfo()
    {

    }

    bool load(const wchar_t& c, unsigned char* dst, uvec2& dims)
    {
        if (!valid)
            return false;

        auto index = FT_Get_Char_Index(face, c);
        if (index == 0)
        {
            std::cerr << "[FreeType] Unrecognized character code: "
                      << std::hex << (int)c << std::endl;
            return false;
        }

        auto err = FT_Load_Glyph(face, index, FT_LOAD_RENDER);
        if (err)
        {
            std::cerr << "[FreeType] Failed to load glyph:"
                      << std::hex << (int)c << std::endl;
            return false;
        }

        FT_GlyphSlotRec* glyph = face->glyph;

        std::cout << "Index = " << glyph->glyph_index << "\n";
        std::cout << "Left = " << glyph->bitmap_left << "\n";
        std::cout << "Top = " << glyph->bitmap_top << "\n";
        std::cout << "Advance x = " << glyph->advance.x << "\n";
        std::cout << "Advance y = " << glyph->advance.y << "\n";

        FT_Bitmap& bitmap = glyph->bitmap;

        std::cout << "Width = " << bitmap.width << "\n";

        memcpy(dst, bitmap.buffer, (size_t)bitmap.rows * (size_t)bitmap.pitch);
        dims.x = bitmap.width;
        dims.y = glyph->bitmap_top;

        return true;
    }
};

Font::Font(const std::string& fn, const size_t& size, const vec3& color) :
    _info(new FontInfo(fn, size)),
    _size(size),
    _color(color)
{
}

Letter Font::get(const wchar_t& c)
{
    unsigned char* buf1 = new unsigned char[_size * _size];
    uvec2 dims;
    _info->load(c, buf1, dims);

    GLfloat* buf2 = new GLfloat[(size_t)dims.x * (size_t)dims.y * 4];

    //std::cout << "Buffer 2 (" << dims.x << "x" << dims.y << "):\n";
    for (size_t r = 0; r < dims.y; ++r)
    {
        for (size_t c = 0; c < dims.x; ++c)
        {
            size_t i1 = r * dims.x + c;
            size_t i2 = 4 * i1;

            buf2[i2 + 0] = _color.r;
            buf2[i2 + 1] = _color.g;
            buf2[i2 + 2] = _color.b;
            buf2[i2 + 3] = buf1[i1] / 255.0f;

            //sstd::cout << " " << buf2[i2 + 3];
        }
        //std::cout << "\n";
    }

    delete[] buf1;

    GLuint id = Textures::generate(buf2, dims, GL_RGBA, GL_FLOAT);

    delete[] buf2;

    return { id, dims };
}

Letter Font::get(const char& c)
{
    return get((wchar_t)c);
}