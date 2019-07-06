
#pragma once

#include "Libs.h"
#include "Textures.h"

#include <string>

class Program
{
public:
    Program(const std::string& vert, const std::string& frag);
    Program(const Program& other) = delete;
    virtual ~Program();

    Program& operator=(const Program& other) = delete;

    const GLuint& ID() const { return _id; }

private:
    void SetMVP(const mat4& mvp);
    void SetTexture(const Texture& tex);

    friend class Window;

private:
    GLuint _id;

    GLint _mvp;
    GLint _texture;
};