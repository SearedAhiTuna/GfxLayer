
#pragma once

#include "Libs.h"

#include <atomic>
#include <memory>
#include <string>

class Renderer;

class Window
{
private:
    Window(GLFWwindow* window);

public:
    
    Window(const Window& other) = delete;
    virtual ~Window();

    Window& operator=(const Window& rhs) = delete;

    void update();
    void markForUpdate();

    bool shouldClose();

    Renderer& getRenderer() { return *_pRenderer; }

private:
    GLFWwindow* _window;
    std::unique_ptr<Renderer> _pRenderer;

    std::atomic<bool> extShouldClose{};
    std::atomic<bool> needsUpdate{};

    friend class Graphics;
};