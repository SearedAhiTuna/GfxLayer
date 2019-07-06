
#include "Shape.h"
#include "Graphics.h"
#include "Textures.h"

#include <iostream>
#include <list>

Shape::Buffer::Buffer() :
    _vbo(0),
    _nverts(0),
    _ndims(0),
    _data(nullptr),
    _size(0),
    _init(false),
    _updated(false),
    _valid(false)
{
}

Shape::Buffer::Buffer(const size_t& nverts, const size_t& ndims) :
    _vbo(0),
    _nverts(nverts),
    _ndims(ndims),
    _data(nullptr),
    _size(0),
    _init(false),
    _updated(false),
    _valid(true)
{
    GLuint temp = 0;
    GRAPHICS_CALL_BEGIN(&temp)
        glGenBuffers(1, &temp);
    GRAPHICS_CALL_END
    _vbo = temp;

    // Generate the data
    _data = new GLfloat[_nverts * _ndims];
    _size = _nverts * _ndims * sizeof(GLfloat);
}

Shape::Buffer::Buffer(Buffer&& other) noexcept :
    _vbo(other._vbo),
    _nverts(other._nverts),
    _ndims(other._ndims),
    _data(other._data),
    _size(other._size),
    _init(other._init),
    _updated(other._updated),
    _valid(true)
{
    other._valid = false;
}

Shape::Buffer::~Buffer()
{
    if (!_valid)
        return;

    GLuint temp = _vbo;
    GRAPHICS_CALL_BEGIN(&temp)
        glDeleteBuffers(1, &temp);
    GRAPHICS_CALL_END

    // Delete the data
    delete[] _data;
}

Shape::Buffer& Shape::Buffer::operator=(Buffer&& rhs) noexcept
{
    _vbo = rhs._vbo;
    _nverts = rhs._nverts;
    _ndims = rhs._ndims;
    _data = rhs._data;
    _size = rhs._size;
    _init = rhs._init;
    _valid = true;

    rhs._valid = false;

    return *this;
}

GLfloat& Shape::Buffer::operator[](const size_t ind)
{
    return _data[ind];
}

Shape::BufferHandle::BufferHandle(Shape& parent, Buffer& buffer):
    _parent(parent),
    _buffer(&buffer)
{
}

void Shape::BufferHandle::Free()
{
    if (_buffer == nullptr)
        return;

    std::lock_guard<std::mutex> myLk(_parent._bigLock);

    auto& allBuffers = _parent._buffers;

    for (auto it = allBuffers.begin(); it != allBuffers.end(); ++it)
    {
        if (it->get() == _buffer)
        {
            allBuffers.erase(it);
            break;
        }
    }
}

Shape::Shape(const size_t nverts, const GLenum& mode) :
    _mode(mode),
    _nverts(nverts)
{
}

Shape::Shape(Shape&& other) noexcept :
    _mode(other._mode),
    _nverts(other._nverts)
{
    // Lock the other shape
    std::lock_guard<std::mutex> lk(other._bigLock);

    // Move all the buffers
    _buffers = other._buffers;

    // Empty the other shape
    other._buffers.clear();

    // Get the texture from the other shape
    _texture = other._texture;
    //other._texture = 0;
}

Shape::~Shape()
{
    Free();
}

Shape& Shape::operator=(Shape&& rhs) noexcept
{
    // Free any data in this shape
    Free();

    // Lock this shape
    std::lock_guard<std::mutex> myLk(_bigLock);

    // Lock the other shape
    std::lock_guard<std::mutex> other(rhs._bigLock);

    // Set the mode
    _mode = rhs._mode;

    // Set the dimensions
    _nverts = rhs._nverts;

    // Move all the buffers
    _buffers = rhs._buffers;

    // Empty the other shape
    rhs._buffers.clear();

    // Get the texture from the other shape
    _texture = rhs._texture;
    //rhs._texture = 0;

    return *this;
}

void Shape::Free()
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    // Empty buffers
    _buffers.clear();

    // Reset program
    _program = -1;
}

Shape::BufferHandle Shape::GenBuffer(const size_t& ndims)
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    Buffer* buf = new Buffer(_nverts, ndims);

    _buffers.emplace_back(buf);
    return BufferHandle(*this, *buf);
}

void Shape::UpdateBuffers()
{
    assertMainThread();

    std::lock_guard<std::mutex> myLk(_bigLock);

    for (auto& ptr : _buffers)
    {
        Buffer& buffer = *ptr.get();

        if (!buffer._updated)
        {
            // Copy the buffer data
            glBindBuffer(GL_ARRAY_BUFFER, buffer._vbo);
            if (buffer._init)
            {
                glBufferSubData(GL_ARRAY_BUFFER, 0, buffer._size, buffer._data);
            }
            else
            {
                glBufferData(GL_ARRAY_BUFFER, buffer._size, buffer._data, GL_STATIC_DRAW);
                buffer._init = true;
            }

            buffer._updated = true;
        }
    }
}

Shape& Shape::Texture(const std::string& fn)
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    _texture = Textures::load(fn);

    return *this;
}

::Texture Shape::Texture()
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    return _texture;
}

Shape& Shape::Program(const int& p)
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    _program = p;

    return *this;
}

int Shape::Program()
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    return _program;
}

Shape& Shape::TF(const mat4& tf)
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    _tf = tf;

    return *this;
}

mat4 Shape::TF()
{
    std::lock_guard<std::mutex> myLk(_bigLock);

    return _tf;
}
