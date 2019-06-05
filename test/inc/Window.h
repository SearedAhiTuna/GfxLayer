
#pragma once

#include "Libs.h"
#include "Renderer.h"

#include <memory>

class Window
{
public:
	Window();
	Window(const Window& other) = delete;
	virtual ~Window();

	Window& operator=(const Window& rhs) = delete;

	void update();

	bool shouldClose();

	Renderer* getRenderer() { return pRenderer.get(); }

private:
	GLFWwindow* window;

	std::unique_ptr<Renderer> pRenderer;
};