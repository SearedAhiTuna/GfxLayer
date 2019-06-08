
#pragma once

#include "Libs.h"
#include "Window.h"

#include <list>
#include <memory>

class Controller
{
public:
	Controller();
	virtual ~Controller();

	Window* createWindow(const int& w, const int& h, const std::string& title = "");
	void updateWindow(Window* w);

	void tick();
	void loop();

	size_t empty();

private:
	void frame();

private:
	struct WindowData
	{
		std::unique_ptr<Window> window{};
		bool needsUpdate{};

		WindowData(Window* _window = nullptr) :
			window(_window)
		{
		}
	};

private:
	std::list<WindowData> windows;
};