
#include "Camera.h"

#include <glm/gtx/transform.hpp>

#include <iostream>

void Camera::tl(const bool& rel, const vec3& d)
{
    std::lock_guard<std::mutex> lk(matMutex);

    if (rel)
        tlMat = glm::translate(glm::mat4(), -d) * tlMat;
    else
        tlMat = glm::translate(glm::mat4(), -d);
}
void Camera::tl(const bool& rel, const vec2& d)
{
    tl(rel, vec3(d, 0));
}

void Camera::rot(const bool& rel, const GLfloat& angle, const vec3& axis)
{
    std::lock_guard<std::mutex> lk(matMutex);

    if (rel)
        rotMat = glm::rotate(mat4(), -angle, axis) * rotMat;
    else
        rotMat = glm::rotate(mat4(), -angle, axis);
}

void Camera::perspective(const GLfloat& fov, const GLfloat& aspect, const GLfloat& min, const GLfloat& max)
{
    std::lock_guard<std::mutex> lk(matMutex);

    pMat = glm::perspective(glm::radians(fov), aspect, min, max);
}

void Camera::orthogonal(const GLfloat& width, const GLfloat & height)
{
    std::lock_guard<std::mutex> lk(matMutex);

    pMat = glm::scale(vec3(width / 2.0f, height / 2.0f, 1.0f));
}

mat4 Camera::getV()
{
    std::lock_guard<std::mutex> lk(matMutex);
    return followMat * rotMat * tlMat;
}
mat4 Camera::getP()
{
    std::lock_guard<std::mutex> lk(matMutex);
    return pMat;
}

vec3 Camera::getPos()
{
    std::lock_guard<std::mutex> lk(matMutex);

    vec4 pos(VEC3_ORIGIN, 1);
    pos = -tlMat * pos;
    return pos.xyz;
}

void Camera::lookAt(const vec3& dst, const bool& pitchEn)
{
    vec3 disp = dst - getPos();
    vec3 dir = glm::normalize(disp);

    GLfloat pitch = asin(dir.y);
    GLfloat yaw = atan2(-dir.x, -dir.z);

    if (pitchEn)
    {
        rot(false, pitch, VEC3_RIGHT);
        rot(true, yaw, VEC3_UP);
    }
    else
    {
        rot(false, yaw, VEC3_UP);
    }
}

void Camera::strafe(const vec3& d)
{
    std::lock_guard<std::mutex> lk(matMutex);

    vec3 newDisp = (rotMat * vec4(d, 1)).xyz;

    tlMat = glm::translate(glm::mat4(), -newDisp) * tlMat;
}

void Camera::strafe(const vec2& d)
{
    strafe(vec3(-d.x, -d.y, 0.0f));
}

void Camera::follow(const bool& rel, const vec3& d)
{
    std::lock_guard<std::mutex> lk(matMutex);

    if (rel)
        followMat = glm::translate(glm::mat4(), -d) * followMat;
    else
        followMat = glm::translate(glm::mat4(), -d);
}

void Camera::follow(const bool& rel, const vec2& d)
{
    follow(rel, vec3(d, 0));
}

void Camera::follow(const bool& rel, const GLfloat& d)
{
    follow(rel, vec3(0, 0, d));
}