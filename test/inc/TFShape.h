
#pragma once

#include "Libs.h"
#include "Shape.h"

#include <list>
#include <memory>
#include <mutex>
#include <vector>

class TFShape : public Shape
{
public:
    template <typename vec, typename Container>
    TFShape(const vec& type, const Container& _verts, const GLenum& mode = GL_TRIANGLES);

    void tfInt(const mat4& mat);

    void tlInt(const vec3& d);
    void tlInt(const vec2& d);

    void rotInt(const GLfloat& angle, const vec3& axis = vec3(0.0f, 0.0f, 1.0f));

    void tfExt(const bool& rel, const mat4& mat);

    void tlExt(const bool& rel, const vec3& d);
    void tlExt(const bool& rel, const vec2& d);

    void rotExt(const bool& rel, const GLfloat& angle, const vec3& axis = vec3(0.0f, 0.0f, 1.0f));

private:
    template <typename vec>
    struct VertBuffer
    {
        std::vector<vec> _verts;
        GLfloat* _vertBuf;
        const size_t _vertSize;
        TFShape& _parent;

        template <typename Container>
        VertBuffer(TFShape& parent, const Container& verts);
        ~VertBuffer();
        
        void UpdateVertex(const size_t& vert);
        void UpdateAllVertices();
    };

    typedef void VertXBuffer;
    typedef VertBuffer<vec2> Vert2Buffer;
    typedef VertBuffer<vec3> Vert3Buffer;
    typedef VertBuffer<vec4> Vert4Buffer;

public:
    template <typename vec, typename Container>
    size_t CreateBuffer(const Container& verts);

    template <typename vec>
    const vec& GetVertex(const size_t& buf, const size_t& vert);

    template <typename vec>
    void SetVertex(const size_t& buf, const size_t& vert, const vec& value);

private:
    std::vector<std::shared_ptr<VertXBuffer>> _buffers;
    size_t _ndims;
};

template <typename vec, typename Container>
TFShape::TFShape(const vec& type, const Container& verts, const GLenum& mode):
    Shape(mode),
    _ndims(type.length())
{
    CreateBuffer<vec, Container>(verts);
}

template <typename vec>
template <typename Container>
TFShape::VertBuffer<vec>::VertBuffer(TFShape& parent, const Container& verts) :
    _vertSize(verts.front().length()),
    _parent(parent)
{
    _verts.reserve(verts.size());
    for (const vec& v : verts)
    {
        _verts.push_back(v);
    }

    _vertBuf = parent.GenBuffer(verts.size(), vertSize);

    UpdateAllVertices();
}

template <typename vec>
TFShape::VertBuffer<vec>::~VertBuffer()
{
    parent.DeleteBuffer(_vertBuf);
}

template <typename vec>
void TFShape::VertBuffer<vec>::UpdateVertex(const size_t& vert)
{
    std::unique_lock<std::mutex> lk(parent._bigLock);

    for (size_t i = 0; i < _vertSize; ++i)
    {
#pragma warning(suppress: 4267)
        _vertBuf[vert * _vertSize + i] = _verts[v][i];
    }

    lk.unlock();

    parent.MarkBufferForUpdate(_vertBuf);
}

template <typename vec>
void TFShape::VertBuffer<vec>::UpdateAllVertices()
{
    std::unique_lock<std::mutex> lk(parent._bigLock);

    for (size_t v = 0; v < _verts.size(); ++v)
    {
        for (size_t i = 0; i < _vertSize; ++i)
        {
#pragma warning(suppress: 4267)
            _vertBuf[v * _vertSize + i] = _verts[v][i];
        }
    }

    lk.unlock();

    parent.MarkBufferForUpdate(_vertBuf);
}

template <typename vec, typename Container>
size_t TFShape::CreateBuffer(const Container& verts)
{
    auto* buf = new VertBuffer<vec>(*this, verts);
    _buffers.emplace_back(buf);
}

template <typename vec>
const vec& TFShape::GetVertex(const size_t& buf, const size_t& vert)
{
    VertBuffer<vec>* buffer = _buffers[buf].get();
    return buffer->_verts[vert];
}

template <typename vec>
void TFShape::SetVertex(const size_t& buf, const size_t& vert, const vec& value)
{
    VertBuffer<vec>* buffer = _buffers[buf].get();
    buffer->_verts[vert] = value;
    buffer->UpdateVertex(vert, value);
}