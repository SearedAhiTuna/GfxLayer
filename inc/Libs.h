
#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#define VEC3_ORIGIN vec3(0, 0, 0)
#define VEC3_UP vec3(0, 1, 0)
#define VEC3_DOWN vec3(0, -1, 0)
#define VEC3_LEFT vec3(-1, 0, 0)
#define VEC3_RIGHT vec3(1, 0, 0)
#define VEC3_FORWARDS vec3(0, 0, -1)
#define VEC3_BACKWARDS vec3(0, 0, 1)

#define UV_TOP_LEFT vec2(0, 0)
#define UV_BOTTOM_LEFT vec2(0, 1)
#define UV_TOP_RIGHT vec2(1, 0)
#define UV_BOTTOM_RIGHT vec2(1, 1)

extern const GLfloat PI;