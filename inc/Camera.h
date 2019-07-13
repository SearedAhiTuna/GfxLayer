
#pragma once

#include "Types.h"

#include <mutex>

class Camera
{
public:
    void tl(const bool& rel, const vec3& d);
    void tl(const bool& rel, const vec2& d);

    void rot(const bool& rel, const GLfloat& angle, const vec3& axis = vec3(0.0f, 0.0f, 1.0f));

    void perspective(const GLfloat& fov, const GLfloat& aspect, const GLfloat& min, const GLfloat& max);
    void orthogonal(const GLfloat& width = 2.0f, const GLfloat & height = 2.0f);

    mat4 getV();
    mat4 getP();

    vec3 getPos();

    void lookAt(const vec3& dst, const bool& pitchEn = true);

    void strafe(const vec3& d);
    void strafe(const vec2& d);

    void follow(const bool& rel, const vec3& d);
    void follow(const bool& rel, const vec2& d);
    void follow(const bool& rel, const GLfloat& d);

private:
    mat4 tlMat{};
    mat4 rotMat{};
    mat4 followMat{};
    mat4 pMat{};

    std::mutex matMutex;
};