
#include "Window.h"
#include "Graphics.h"
#include "Renderer.h"

Window::Window(GLFWwindow* window):
    _window(window)
{
    // Create renderer
    _pRenderer = std::make_unique<Renderer>();
}

Window::~Window()
{
    assertMainThread();
    glfwDestroyWindow(_window);
}

void Window::update()
{
    assertMainThread();

    // Set window as current context
    glfwMakeContextCurrent(_window);

    // Clear the window
    glClear(GL_COLOR_BUFFER_BIT);

    // Render
    if (_pRenderer.get())
        _pRenderer->render();

    // Swap the buffers
    glfwSwapBuffers(_window);
}

void Window::markForUpdate()
{
    needsUpdate.store(true);
    glfwPostEmptyEvent();
}

bool Window::shouldClose()
{
    if (onMainThread())
        return glfwWindowShouldClose(_window);
    else
    {
        return extShouldClose.load();
    }
}