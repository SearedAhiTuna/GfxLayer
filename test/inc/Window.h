
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

	Renderer* getRenderer() { return pRenderer.get(); }

private:
	GLFWwindow* window;

	std::unique_ptr<Renderer> pRenderer;
};