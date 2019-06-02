
#include "Libs.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

#include <glm/gtc/constants.hpp>

const GLfloat PI = pi<GLfloat>();

void glfwInitialize()
{
	glfwInit();
}

std::atomic<bool> glewIsInitialized;

void glewInitialize()
{
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		throw GraphicsError("Failed to initialize GLEW");
	else
		glewIsInitialized.store(true);
}

static std::mutex glBufferMutex;

void glStartBufferTX(const GLuint& id)
{
	while (!glewIsInitialized.load());

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
	while (!glewIsInitialized.load());

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

void sleepMS(unsigned int t)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(t));
}