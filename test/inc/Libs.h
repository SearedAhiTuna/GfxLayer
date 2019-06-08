
#pragma once

#include <stdexcept>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>

using namespace glm;

class GraphicsError : public std::runtime_error
{
public:
	GraphicsError(const std::string& msg) :
		runtime_error(msg)
	{
	}
};

extern const GLfloat PI;

extern const vec3 VEC3_ORIGIN;
extern const vec3 VEC3_UP;
extern const vec3 VEC3_DOWN;
extern const vec3 VEC3_LEFT;
extern const vec3 VEC3_RIGHT;
extern const vec3 VEC3_FORWARDS;
extern const vec3 VEC3_BACKWARDS;

extern void glfwInitialize();

extern void glewInitialize();

extern void glStartBufferTX(const GLuint& vbo);
extern void glEndBufferTX();
extern bool glOngoingBufferTX();

extern void glStartVertexArrayTX(const GLuint& vao);
extern void glEndVertexArrayTX();
extern bool glOngoingVertexArrayTX();

extern void glfwStartWindowTX(GLFWwindow* window);
extern void glfwEndWindowTX();
extern bool glfwOngoingWindowTX();

extern void sleepMS(unsigned int t);