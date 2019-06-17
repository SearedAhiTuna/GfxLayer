
#include "Shape.h"
#include "Graphics.h"

#include <iostream>
#include <list>

static std::list<GLuint> used{};

Shape::Buffer::Buffer(const size_t& _nverts, const size_t& _ndims):
    nverts(_nverts),
    ndims(_ndims)
{
    GLuint temp = 0;
    GRAPHICS_CALL_BEGIN(&temp)
        glGenBuffers(1, &temp);
    GRAPHICS_CALL_END
    vbo = temp;

    // Generate the data
    data = new GLfloat[nverts * ndims];
    size = nverts * ndims * sizeof(GLfloat);
}

Shape::Buffer::~Buffer()
{
    GLuint temp = vbo;
    GRAPHICS_CALL_BEGIN(&temp)
        glDeleteBuffers(1, &temp);
    GRAPHICS_CALL_END

    // Delete the data
    delete[] data;
}

Shape::Shape(const GLenum& _mode):
    mode(_mode)
{
}

Shape::~Shape()
{
    if (textureID != 0)
    {
        freeTexture(this->textureID);
    }
}

GLfloat* Shape::genBuffer(const size_t& nverts, const size_t& ndims)
{
    std::lock_guard<std::mutex> lk(buffersMutex);
    buffers.emplace_back(nverts, ndims);
    needsUpdate = true;
    return buffers.back().data;
}

void Shape::deleteBuffer(GLfloat* buf)
{
    std::lock_guard<std::mutex> lk(buffersMutex);
    for (auto it = buffers.begin(); it != buffers.end(); ++it)
    {
        if (it->data == buf)
        {
            buffers.erase(it);
            break;
        }
    }
    needsUpdate = true;
}

void Shape::update()
{
    std::lock_guard<std::mutex> lk(buffersMutex);
    needsUpdate = true;
}

void Shape::draw()
{
    // This function should only be called by the main thread
    assertMainThread();

    std::lock_guard<std::mutex> lk(buffersMutex);

    if (buffers.empty())
    {
        return;
    }

    // Update buffers
    if (needsUpdate)
    {
        for (Buffer& buf : buffers)
        {
            // Copy the buffer data
            glBindBuffer(GL_ARRAY_BUFFER, buf.vbo);
            if (buf.init)
            {
                glBufferSubData(GL_ARRAY_BUFFER, 0, buf.size, buf.data);
            }
            else
            {
                glBufferData(GL_ARRAY_BUFFER, buf.size, buf.data, GL_STATIC_DRAW);
                buf.init = true;
            }

        }

        needsUpdate = false;
    }

    // For each buffer
    GLuint index = 0;
    for (Buffer& buf : buffers)
    {
        // Enable this attribute array
        glEnableVertexAttribArray(index);

        // Bind the buffer
        glBindBuffer(GL_ARRAY_BUFFER, buf.vbo);

        // Set the attribute pointer
        glVertexAttribPointer(
            index,              // attribute.
            (GLint)buf.ndims,    // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // Increment index
        ++index;
    }

    // Draw the arrays
    glDrawArrays(mode, 0, (GLsizei)buffers.front().nverts);

    // Disable the attribute arrays
    for (GLuint i = 0; i < index; ++i)
    {
        glDisableVertexAttribArray(i);
    }
}

Shape& Shape::setTexture(const std::string& fn)
{
    if (textureID != 0)
        freeTexture(textureID);
    textureID = loadTexture(fn);
    std::cout << "Got texture ID " << textureID << std::endl;
    return *this;
}