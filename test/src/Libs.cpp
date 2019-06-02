
#include "Libs.h"

#include <mutex>

static std::mutex glBufferMutex;

void glStartBufferTX(const GLuint& id)
{
	glBufferMutex.lock();
	glBindBuffer(GL_ARRAY_BUFFER, id);
}
void glEndBufferTX()
{
	glBufferMutex.unlock();
}
bool glOngoingBufferTX()
{
	if (glBufferMutex.try_lock())
	{
		glBufferMutex.unlock();
		return false;
	}
	else
	{
		return true;
	}
}

static std::mutex glVertexArrayMutex;

void glStartVertexArrayTX(const GLuint& id)
{
	glVertexArrayMutex.lock();
	glBindVertexArray(id);
}
void glEndVertexArrayTX()
{
	glVertexArrayMutex.unlock();
}
bool glOngoingVertexArrayTX()
{
	if (glVertexArrayMutex.try_lock())
	{
		glVertexArrayMutex.unlock();
		return false;
	}
	else
	{
		return true;
	}
}

static std::mutex glfwWindowMutex;

void glfwStartWindowTX(GLFWwindow* window)
{
	glfwWindowMutex.lock();
	glfwMakeContextCurrent(window);
}
void glfwEndWindowTX()
{
	glfwWindowMutex.unlock();
}
bool glfwOngoingWindowTX()
{
	if (glfwWindowMutex.try_lock())
	{
		glfwWindowMutex.unlock();
		return false;
	}
	else
	{
		return true;
	}
}