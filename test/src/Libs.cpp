
#include "Libs.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

#include <glm/gtc/constants.hpp>

const GLfloat PI = pi<GLfloat>();

const vec3 VEC3_ORIGIN = vec3(0, 0, 0);
const vec3 VEC3_UP = vec3(0, 1, 0);
const vec3 VEC3_DOWN = vec3(0, -1, 0);
const vec3 VEC3_LEFT = vec3(-1, 0, 0);
const vec3 VEC3_RIGHT = vec3(1, 0, 0);
const vec3 VEC3_FORWARDS = vec3(0, 0, -1);
const vec3 VEC3_BACKWARDS = vec3(0, 0, 1);

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