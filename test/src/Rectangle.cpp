
#include "Rectangle.h"

Rectangle::Rectangle(const vec3& v1, const vec3& v2) :
    Shape(4, GL_TRIANGLE_STRIP)
{
    auto buf = GenBuffer(3);
    buf.Write(VEC3_ORIGIN, 0);
    buf.Write(v1, 1);
    buf.Write(v2, 2);
    buf.Write(v1 + v2, 3);

    _normal = vec4(normalize(cross(v1, v2)), 0);
}

void Rectangle::GenUV()
{
    auto buf = GenBuffer(2);
    buf.Write(UV_BOTTOM_LEFT, 0);
    buf.Write(UV_BOTTOM_RIGHT, 1);
    buf.Write(UV_TOP_LEFT, 2);
    buf.Write(UV_TOP_RIGHT, 3);
}

void Rectangle::GenNormals()
{
    auto buf = GenBuffer(4);
    for (size_t i = 0; i < 4; ++i)
    {
        buf.Write(_normal, i);
    }
}