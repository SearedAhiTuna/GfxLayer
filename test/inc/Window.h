
#pragma once

#include "Libs.h"
#include "Renderer.h"

#include <memory>

class Window
{
public:
	Window();
	virtual ~Window();

	void update();

	bool shouldClose();

private:
	GLFWwindow* window;

	std::unique_ptr<Renderer> pRenderer;
};