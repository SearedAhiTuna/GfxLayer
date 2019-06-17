
#pragma once

#include "Libs.h"
#include "Shape.h"

#include <list>
#include <memory>
#include <vector>

class TFShape : public Shape
{
public:
    TFShape(const std::vector<vec4>& _verts, const GLenum& mode = GL_TRIANGLES);
    TFShape(const std::vector<vec3>& _verts, const GLenum& mode = GL_TRIANGLES);
    TFShape(const std::vector<vec2>& _verts, const GLenum& mode = GL_TRIANGLES);

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
        std::vector<vec> verts;
        GLfloat* vertBuf;
        const size_t vertSize;
        TFShape* parent;

        VertBuffer(TFShape* _parent, const std::vector<vec>& _verts):
            verts(_verts),
            vertSize(_verts[0].length()),
            parent(_parent)
        {
            vertBuf = parent->genBuffer(verts.size(), vertSize);
        }

        ~VertBuffer()
        {
            parent->deleteBuffer(vertBuf);
        }

        void update()
        {
            for (size_t v = 0; v < verts.size(); ++v)
            {
                for (size_t i = 0; i < vertSize; ++i)
                {
#pragma warning(suppress: 4267)
                    vertBuf[v * vertSize + i] = verts[v][i];
                }
            }
        }
        
    };

    typedef void VertXBuffer;
    typedef VertBuffer<vec4> Vert4Buffer;
    typedef VertBuffer<vec3> Vert3Buffer;
    typedef VertBuffer<vec2> Vert2Buffer;

public:
    template <typename vec>
    size_t bufferCreate(std::vector<vec> verts)
    {
        VertBuffer<vec>* buf = new VertBuffer<vec>(this, verts);
        buffers.emplace_back(buf);
        buf->update();
        update();
        return buffers.size() - 1;
    }

    template <typename vec>
    const vec& bufferGet(const size_t& bufInd, const size_t& vertInd)
    {
        VertBuffer<vec>& buf = *(VertBuffer<vec>*)buffers[bufInd].get();
        return buf.verts[vertInd];
    }

    template <typename vec>
    void bufferSet(const size_t& bufInd, const size_t& vertInd, const vec& value)
    {
        VertBuffer<vec>& buf = *(VertBuffer<vec>*)buffers[bufInd].get();
        buf->verts[vertInd] = value;
        buf->update();
        update();
    }

private:
    std::vector<std::shared_ptr<VertXBuffer>> buffers;
};