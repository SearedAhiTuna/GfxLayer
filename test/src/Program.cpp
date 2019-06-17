
#include "Program.h"

#include "Graphics.h"
#include "Shader.h"

Program::Program(const std::string& vert, const std::string& frag)
{
    Shader vertShader(GL_VERTEX_SHADER, vert);
    Shader fragShader(GL_FRAGMENT_SHADER, frag);

    GLuint id_temp;
    GRAPHICS_CALL_BEGIN(&id_temp)
        id_temp = glCreateProgram();
    GRAPHICS_CALL_END
    id = id_temp;

    GLuint fsID = fragShader.getID();
    GLuint vsID = vertShader.getID();

    GRAPHICS_CALL_BEGIN(id_temp, fsID, vsID)
        glAttachShader(id_temp, fsID);
        glAttachShader(id_temp, vsID);

        glLinkProgram(id_temp);

        glDetachShader(id_temp, fsID);
        glDetachShader(id_temp, vsID);
    GRAPHICS_CALL_END
}

Program::~Program()
{
    if (onMainThread())
    {
        glDeleteShader(id);
    }
    else
    {
        GLuint id_temp = id;
        GRAPHICS_CALL_BEGIN(id_temp)
            glDeleteShader(id_temp);
        GRAPHICS_CALL_END
    }
    
}