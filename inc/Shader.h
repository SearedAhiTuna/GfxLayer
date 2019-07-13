
#pragma once

#include "Types.h"

#include <string>

class Shader
{
public:
    Shader(const GLenum& shaderType, const std::string& fn);
    Shader(const Shader& other) = delete;
    virtual ~Shader();

    Shader& operator=(const Shader& rhs) = delete;

    const GLuint& getID() { return id; }

private:
    GLuint id{};
};