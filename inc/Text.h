
#pragma once

#include "Font.h"
#include "Types.h"
#include "Rectangle.h"

#include <list>
#include <string>

class Text : protected std::list<Rectangle>
{
public:
    Text(Font& fnt, const vec3& hor, const vec3& ver, const std::string text);

    Text& Program(const int& p);

    Text& TF(const bool& relative, const mat4& tf);

    std::list<Rectangle>::iterator begin();

    std::list<Rectangle>::iterator end();
};