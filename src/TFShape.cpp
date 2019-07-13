
#include "TFShape.h"

#include <glm/gtx/transform.hpp>

void TFShape::tfInt(const mat4& mat)
{
    if (_ndims == 2)
    {
        Vert2Buffer* buf = reinterpret_cast<Vert2Buffer*>(_buffers.front().get());

        for (vec2& v : buf->_verts)
        {
            v = (mat * vec4(v, 0, 1)).xy;
        }
    }
    else if (_ndims == 3)
    {
        Vert3Buffer* buf = reinterpret_cast<Vert3Buffer*>(_buffers.front().get());

        for (vec3& v : buf->_verts)
        {
            v = (mat * vec4(v, 1)).xyz;
        }
    }
    else if (_ndims == 4)
    {
        Vert4Buffer* buf = reinterpret_cast<Vert4Buffer*>(_buffers.front().get());

        for (vec4& v : buf->_verts)
        {
            v = mat * v;
        }
    }
}

void TFShape::tlInt(const vec3& d)
{
    tfInt(glm::translate(glm::mat4(), d));
}
void TFShape::tlInt(const vec2& d)
{
    tfInt(glm::translate(mat4(), vec3(d.x, d.y, 0.0f)));
}

void TFShape::rotInt(const GLfloat& angle, const vec3& axis)
{
    tfInt(glm::rotate(mat4(), angle, axis));
}

void TFShape::tfExt(const bool& rel, const mat4& mat)
{
    if (rel)
        _tf = _tf * mat;
    else
        _tf = mat;
}

void TFShape::tlExt(const bool& rel, const vec3& d)
{
    tfExt(rel, glm::translate(glm::mat4(), d));
}
void TFShape::tlExt(const bool& rel, const vec2& d)
{
    tfExt(rel, glm::translate(mat4(), vec3(d.x, d.y, 0.0f)));
}

void TFShape::rotExt(const bool& rel, const GLfloat& angle, const vec3& axis)
{
    tfExt(rel, glm::rotate(mat4(), angle, axis));
}
