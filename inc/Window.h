
#pragma once

#include "Camera.h"
#include "Types.h"
#include "Mouse.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

class Program;
class Shape;

class Window final
{
private:
    Window();
    Window(GLFWwindow* window);
    Window(const Window& other) = delete;
    Window(Window&& other) noexcept;

public:
    ~Window();

private:
    Window& operator=(const Window& rhs) = delete;
    Window& operator=(Window&& rhs) noexcept;

    void Free();

    void Update();
    void Draw(Shape& s);

public:
    void MarkForUpdate();
    bool ShouldClose();

    void RegisterShape(Shape& s);
    void DeregisterShape(Shape& s);

    template <class ShapeGroup>
    void RegisterShapes(ShapeGroup& ss)
    {
        for (Shape& s : ss)
            RegisterShape(s);
    }

    template <class ShapeGroup>
    void DeregisterShapes(ShapeGroup& ss)
    {
        for (Shape& s : ss)
            DeregisterShape(s);
    }

    void AddProgram(const std::string& vert, const std::string& frag);

    Camera& Camera() { return *_camera; }

    Mouse& Mouse() { return *_mouse; }

private:
    Program* UseProgram(const int& index);

    friend class Graphics;

private:
    GLFWwindow* _window;

    std::atomic<bool> _updated{};
    std::atomic<bool> _extShouldClose{};

    GLuint _vao;

    std::unordered_set<Shape*> _shapes;
    std::mutex _shapeLock;

    std::vector<std::unique_ptr<Program>> _programs;
    int _curProgram{ -1 };
    std::mutex _programLock;

    std::unique_ptr<::Camera> _camera;

    std::unique_ptr<::Mouse> _mouse;
};