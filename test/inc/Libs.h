
#pragma once

#include <stdexcept>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
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