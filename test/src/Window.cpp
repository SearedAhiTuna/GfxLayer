
#include "Window.h"

#include "Renderer.h"

Window::Window(const int& w, const int& h, const std::string& title)
{
	// Initialize GLFW
	glfwInitialize();

	window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
	if (!window)
		throw GraphicsError("Failed to create GLFW window");

	// Set window as current context
	glfwStartWindowTX(window);

	// Initialize GLEW
	glewInitialize();

	// Set sticky
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Create renderer
	pRenderer = std::make_unique<Renderer>();

	// End transaction
	glfwEndWindowTX();
}

Window::~Window()
{
	glfwDestroyWindow(window);
}

void Window::update()
{
	// Set window as current context
	glfwStartWindowTX(window);

	// Clear the window
	glClear(GL_COLOR_BUFFER_BIT);

	// Render
	if (pRenderer.get())
		pRenderer->render();

	// Swap the buffers
	glfwSwapBuffers(window);

	// End transaction
	glfwEndWindowTX();
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(window);
}