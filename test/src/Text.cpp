
#include "Text.h"

#include <iostream>

Text::Text(Font& fnt, const vec3& hor, const vec3& ver, const std::string text)
{
    GLfloat height = (GLfloat)fnt.size();

    vec3 offset = VEC3_ORIGIN;

    char prev = '\0';
    for (const char& c : text)
    {
        Letter l = fnt.get(c);
        std::cout << "Texture = " << l.tex << "\n";

        vec3 v1 = (l.dims.x / height) * hor;
        vec3 v2 = (l.dims.y / height) * ver;

        emplace_back(v1, v2);
        Rectangle& r = back();

        r.GenUV();
        r.GenNormals();
        r.Texture(l.tex);

        if (prev)
        {
            GLuint kern = fnt.kerning(prev, c);
            offset += (kern / height) * hor;
            std::cout << "Kern=" << kern << "\n";
        }

        r.TF(true, translate(offset));
        offset += v1;

        prev = c;
    }
}

Text& Text::Program(const int& p)
{
    for (Shape& s : *this)
    {
        s.Program(p);
    }

    return *this;
}

Text& Text::TF(const bool& relative, const mat4& tf)
{
    for (Shape& s : *this)
    {
        s.TF(relative, tf);
    }

    return *this;
}

std::list<Rectangle>::iterator Text::begin()
{
    return std::list<Rectangle>::begin();
}

std::list<Rectangle>::iterator Text::end()
{
    return std::list<Rectangle>::end();
}
