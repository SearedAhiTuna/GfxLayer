
#include "Shader.h"
#include "Graphics.h"
#include "InternalLibs.h"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const GLenum& shaderType, const std::string& fn)
{
    // Create the shader
    GLuint id_temp;
    GRAPHICS_CALL_BEGIN(&id_temp, shaderType)
        id_temp = glCreateShader(shaderType);
    GRAPHICS_CALL_END
    id = id_temp;

    // Read the shader from the file
    std::string code;
    std::ifstream file(fn, std::ios::in);
    if (file.is_open())
    {
        std::stringstream ss;
        ss << file.rdbuf();
        code = ss.str();
        file.close();
    }
    else
    {
        throwGraphicsError("Failed to open shader file");
    }

    // Compile the shader
    char const* source = code.c_str();
    GRAPHICS_CALL_BEGIN(id_temp, &source)
        glShaderSource(id_temp, 1, &source, NULL);
        glCompileShader(id_temp);
    GRAPHICS_CALL_END

    // Check if the shader compiled
    GLint status;
    GRAPHICS_CALL_BEGIN(id_temp, &status)
        glGetShaderiv(id_temp, GL_COMPILE_STATUS, &status);
    GRAPHICS_CALL_END

    if (status == GL_FALSE)
    {
        GLint logSize;
        GRAPHICS_CALL_BEGIN(id_temp, &logSize)
            glGetShaderiv(id_temp, GL_INFO_LOG_LENGTH, &logSize);
        GRAPHICS_CALL_END

        char* log_contents = new char[logSize];
        GRAPHICS_CALL_BEGIN(id_temp, logSize, log_contents)
            glGetShaderInfoLog(id_temp, logSize, NULL, log_contents);
        GRAPHICS_CALL_END
        
        std::cerr << "Shader compilation failed!\n";
        std::cerr << log_contents;
        std::cerr.flush();

        delete[] log_contents;

        throwGraphicsError("Shader compilation failed: See error log");
    }
}

Shader::~Shader()
{
    GLuint id_temp = id;
    GRAPHICS_CALL_BEGIN(id_temp)
        glDeleteShader(id_temp);
    GRAPHICS_CALL_END
}