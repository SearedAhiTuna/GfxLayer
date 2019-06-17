
#include "Renderer.h"
#include "Camera.h"
#include "Graphics.h"
#include "Shape.h"

#include <iostream>

#define MVP_MATRIX "MVP"
#define TEXTURE_SAMPLER "TEXTURE_SAMPLER"

Renderer::Renderer()
{
    GLuint temp = 0;

    if (onMainThread())
    {
        glGenVertexArrays(1, &temp);
    }
    else
    {
        GLuint temp;
        GRAPHICS_CALL_BEGIN(&temp)
            glGenVertexArrays(1, &temp);
        GRAPHICS_CALL_END
    }

    vao = temp;

    camera = std::make_unique<Camera>();
}

Renderer::~Renderer()
{
    assertMainThread();
    glDeleteVertexArrays(1, &vao);
}

void Renderer::addShape(Shape& shape)
{
    std::lock_guard<std::mutex> lk(shapesMutex);
    shapes.emplace(&shape);
}

void Renderer::removeShape(Shape& shape)
{
    std::lock_guard<std::mutex> lk(shapesMutex);
    shapes.erase(&shape);
}

void Renderer::render()
{
    // This function should only be called by the main thread
    assertMainThread();

    // Bind the VAO
    glBindVertexArray(vao);

    // Draw all shapes
    std::lock_guard<std::mutex> lk(shapesMutex);
    for (Shape* shape : shapes)
    {
        if (shape->usesProgram())
        {
            GLuint progID = useProgram(shape->getProgramIndex());

            if (progID)
            {
                GLint mvpID = glGetUniformLocation(progID, MVP_MATRIX);

                if (mvpID != -1)
                {
                    mat4 mvpMat{};

                    mvpMat = shape->getTF() * mvpMat;

                    mvpMat = camera->getV() * mvpMat;

                    mvpMat = camera->getP() * mvpMat;

                    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvpMat[0][0]);
                }

                if (shape->getTexture())
                {
                    GLuint textureSampler = glGetUniformLocation(progID, TEXTURE_SAMPLER);

                    if (textureSampler != -1)
                    {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, shape->getTexture());
                        glUniform1i(textureSampler, 0);
                    }
                }
            }
        }
        else
        {
            useProgram(-1);
        }

        shape->draw();
    }
}

void Renderer::addProgram(const std::string& vert, const std::string& frag)
{
    programs.push_back(std::make_unique<Program>(vert, frag));
}

GLuint Renderer::useProgram(const int& index)
{
    if (index == -1)
    {
        if (index != curProgram)
        {
            curProgram = -1;
            glUseProgram(0);
        }
        
        return 0;
    }
    else
    {
        if (index != curProgram)
        {
            curProgram = index;
            glUseProgram(programs[curProgram]->getID());
        }
        
        return programs[curProgram]->getID();
    }
}