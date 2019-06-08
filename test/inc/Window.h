
#pragma once

#include "Libs.h"

#include <memory>

class Renderer;

class Window
{
public:
	Window(const int& w, const int& h, const std::string& title = "");
	Window(const Window& other) = delete;
	virtual ~Window();

	Window& operator=(const Window& rhs) = delete;

	void update();

	bool shouldClose();

	Renderer& getRenderer() { return *pRenderer; }

private:
	GLFWwindow* window;

	std::unique_ptr<Renderer> pRenderer;
};