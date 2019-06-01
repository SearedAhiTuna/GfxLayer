
#include "Graphics.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
	for (auto it = screens.begin(); it != screens.end(); ++ it)
	{
		Screen& scr = it->second;
		glfwDestroyWindow(scr.window);
	}
}

ScreenID Graphics::createWindow(const int& w, const int& h, const std::string& title, const bool& fs)
{
	std::lock_guard<std::mutex> lk(glfwLock);

	if (!glfwInit())
	{
		throw GraphicsError("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(w, h, title.c_str(), fs ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	if (!window)
	{
		throw GraphicsError("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(window);
		throw GraphicsError("Failed to initialize GLEW");
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	screens[nextID] = Screen(window);

	return nextID++;
}

void Graphics::loop()
{
	while (!screens.empty())
	{
		glfwWaitEvents();

		// For every screen
		for (auto it = screens.begin(); it != screens.end();)
		{
			Screen& scr = it->second;

			std::lock_guard<std::mutex> lk(glfwLock);
			glfwMakeContextCurrent(scr.window);

			if (scr.redraw)
			{
				glfwSwapBuffers(scr.window);

				scr.redraw = false;
			}

			if (glfwWindowShouldClose(scr.window))
			{
				glfwDestroyWindow(scr.window);
				it = screens.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void Graphics::redraw(const ScreenID& screen)
{
	Screen& scr = screen == 0 ? screens.begin()->second : screens[screen];

	std::lock_guard<std::mutex> lk(glfwLock);
	glfwMakeContextCurrent(scr.window);

	scr.redraw = true;

	glClear(GL_COLOR_BUFFER_BIT);

	glfwPostEmptyEvent();
}

void Graphics::setClearColor(const vec4& color, const ScreenID& screen)
{
	Screen& scr = screen == 0 ? screens.begin()->second : screens[screen];

	std::lock_guard<std::mutex> lk(glfwLock);
	glfwMakeContextCurrent(scr.window);

	glClearColor(color.x, color.y, color.z, color.w);
}