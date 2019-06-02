
#include "Window.h"

Window::Window()
{
	if (!glfwInit())
		throw GraphicsError("Failed to initialize GLFW");

	window = glfwCreateWindow(1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if (!window)
		throw GraphicsError("Failed to create GLFW window");

	// Set window as current context
	glfwStartWindowTX(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		throw GraphicsError("Failed to initialize GLEW");

	// Set sticky
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

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