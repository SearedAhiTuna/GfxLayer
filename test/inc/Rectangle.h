
#pragma once

#include "Shape.h"

class Rectangle : public Shape
{
public:
    Rectangle(const vec3& v1, const vec3& v2);

    void GenUV();
    void GenNormals();

private:
    vec3 _normal;
};