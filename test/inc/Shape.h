
#pragma once

#include "Libs.h"
#include "Textures.h"

#include <list>
#include <memory>
#include <mutex>

class Shape
{
private:
    struct Buffer
    {
        GLuint _vbo;

        size_t _nverts;
        size_t _ndims;

        GLfloat* _data;
        size_t _size;

        bool _init;
        bool _updated;
        bool _valid;

        Buffer();
        Buffer(const size_t& nverts, const size_t& ndims);
        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) noexcept;
        ~Buffer();

        Buffer& operator=(const Buffer& rhs) = delete;
        Buffer& operator=(Buffer&& rhs) noexcept;

        GLfloat& operator[](const size_t ind);
    };

    class BufferHandle
    {
    private:
        BufferHandle(Shape& parent, Buffer& buffer);

    public:
        template <typename vec>
        BufferHandle& Write(const vec& value, const size_t& ind);

        template <typename Container>
        BufferHandle& WriteRange(const Container& values, const size_t& start = 0);

        void Free();

    private:
        friend class Shape;

    private:
        Shape& _parent;
        Buffer* _buffer;
    };

public:
    Shape(const size_t nverts = 0, const GLenum& mode = GL_TRIANGLES);
    Shape(const Shape& other) = delete;
    Shape(Shape&& other) noexcept;
    virtual ~Shape();

    Shape& operator=(const Shape& rhs) = delete;
    Shape& operator=(Shape&& rhs) noexcept;

private:
    void Free();

public:
    BufferHandle GenBuffer(const size_t& ndims);

    Shape& Texture(const std::string& fn);
    ::Texture Texture();

    Shape& Program(const int& p);
    int Program();
    bool UsesProgram() { return Program() >= 0; }

    Shape& TF(const mat4& tf);
    mat4 TF();

    const GLenum& Mode() { return _mode; }

    const size_t& NumVerts() { return _nverts; }

private:
    void UpdateBuffers();

    friend class Window;

protected:
    std::mutex _bigLock;

    GLenum _mode;

    size_t _nverts;

    std::list<std::shared_ptr<Buffer>> _buffers;

    int _program{ -1 };

    ::Texture _texture{};

    mat4 _tf {};
};

template <typename vec>
Shape::BufferHandle& Shape::BufferHandle::Write(const vec& value, const size_t& ind)
{
    if (_buffer == nullptr)
        return *this;

    std::lock_guard<std::mutex> myLk(_parent._bigLock);

    const size_t& ndims = _buffer->_ndims;

    for (size_t i = 0; i < value.length(); ++i)
    {
#pragma warning(suppress: 4267)
        _buffer->_data[ind * ndims + i] = value[i];
    }

    _buffer->_updated = false;

    return *this;
}

template <typename Container>
Shape::BufferHandle& Shape::BufferHandle::WriteRange(const Container& values, const size_t& start)
{
    if (_buffer == nullptr)
        return *this;

    std::lock_guard<std::mutex> myLk(_parent._bigLock);

    const size_t& ndims = _buffer->_ndims;

    size_t ind = start;
    for (const auto& value : values)
    {
        for (size_t i = 0; i < value.length(); ++i)
        {
#pragma warning(suppress: 4267)
            _buffer->_data[ind * ndims + i] = value[i];
        }
        ++ind;
    }

    _buffer->_updated = false;

    return *this;
}