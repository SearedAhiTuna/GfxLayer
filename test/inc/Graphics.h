
#pragma once

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

class GraphicsError : public std::runtime_error
{
public:
	GraphicsError(const std::string& msg):
		runtime_error(msg)
	{
	}
};

typedef unsigned int ScreenID;

class Graphics final
{
public:
	Graphics();
	~Graphics();

	ScreenID createWindow(const int& w = 640, const int& h = 480, const std::string& title = "", const bool& fs = false);

	void loop();

	void redraw(const ScreenID& screen = 0);

	void setClearColor(const vec4& color, const ScreenID& screen = 0);

private:
	struct Screen
	{
		GLFWwindow* window{};
		bool redraw{};

		Screen(GLFWwindow* _window = nullptr):
			window(_window)
		{
		}
	};

private:
	ScreenID nextID{ 1 };
	std::map<ScreenID, Screen> screens;

	std::mutex glfwLock{};
};