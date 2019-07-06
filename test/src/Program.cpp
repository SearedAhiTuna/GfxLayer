
#include "Program.h"

#include "Graphics.h"
#include "Shader.h"

#define MVP_MATRIX "MVP"
#define TEXTURE_SAMPLER "TEXTURE_SAMPLER"

Program::Program(const std::string& vert, const std::string& frag)
{
    Shader vertShader(GL_VERTEX_SHADER, vert);
    Shader fragShader(GL_FRAGMENT_SHADER, frag);

    GLuint id_temp;
    GRAPHICS_CALL_BEGIN(&id_temp)
        id_temp = glCreateProgram();
    GRAPHICS_CALL_END
    _id = id_temp;

    GLuint fsID = fragShader.getID();
    GLuint vsID = vertShader.getID();

    GRAPHICS_CALL_BEGIN(id_temp, fsID, vsID)
        glAttachShader(id_temp, fsID);
        glAttachShader(id_temp, vsID);

        glLinkProgram(id_temp);

        glDetachShader(id_temp, fsID);
        glDetachShader(id_temp, vsID);
    GRAPHICS_CALL_END

    GLint mvp_temp;
    GRAPHICS_CALL_BEGIN(id_temp, &mvp_temp)
        mvp_temp = glGetUniformLocation(id_temp, MVP_MATRIX);
    GRAPHICS_CALL_END
    _mvp = mvp_temp;

    GLint texture_temp;
    GRAPHICS_CALL_BEGIN(id_temp, &texture_temp)
        texture_temp = glGetUniformLocation(id_temp, TEXTURE_SAMPLER);
    GRAPHICS_CALL_END
    _texture = texture_temp;
}

Program::~Program()
{
    if (onMainThread())
    {
        glDeleteShader(_id);
    }
    else
    {
        GLuint id_temp = _id;
        GRAPHICS_CALL_BEGIN(id_temp)
            glDeleteShader(id_temp);
        GRAPHICS_CALL_END
    }
    
}

void Program::SetMVP(const mat4& mvp)
{
    if (_mvp != -1)
    {
        glUniformMatrix4fv(_mvp, 1, GL_FALSE, &mvp[0][0]);
    }
}

void Program::SetTexture(const Texture& tex)
{
    if (_texture != -1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glUniform1i(_texture, 0);
    }
}