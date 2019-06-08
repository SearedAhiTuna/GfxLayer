
#include "Controller.h"

#include <iostream>

Controller::Controller()
{
}

Controller::~Controller()
{
}

Window* Controller::createWindow(const int& w, const int& h, const std::string& title)
{
	Window* window = new Window(w, h, title);
	windows.emplace_back(window);
	return window;
}

void Controller::updateWindow(Window* w)
{
	for (WindowData& wd : windows)
	{
		if (wd.window.get() == w)
		{
			wd.needsUpdate = true;
			break;
		}
	}

	glfwPostEmptyEvent();
}

void Controller::tick()
{
	frame();
	glfwPollEvents();
}

void Controller::loop()
{
	while (!windows.empty())
	{
		frame();
		glfwWaitEvents();
	}
}

size_t Controller::empty()
{
	return windows.empty();
}

void Controller::frame()
{
	for (auto it = windows.begin(); it != windows.end();)
	{
		WindowData& wd = *it;
		Window* w = wd.window.get();

		if (wd.needsUpdate)
		{
			w->update();
			wd.needsUpdate = false;
		}

		if (w->shouldClose())
			it = windows.erase(it);
		else
			++it;
	}
}