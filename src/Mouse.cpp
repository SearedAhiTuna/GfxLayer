
#include "Mouse.h"
#include "InternalLibs.h"

#include <unordered_map>

#include <iostream>

static std::unordered_map<GLFWwindow*, Mouse*> mice;

void cursor_position_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mice[window]->callPosCallbacks(xoffset, yoffset);
}

void cursor_enter_callback(GLFWwindow* window, int entered)
{
    mice[window]->callEnterCallbacks(entered);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    mice[window]->callButtonCallbacks(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mice[window]->callScrollCallbacks(xoffset, yoffset);
}

Mouse::Mouse(GLFWwindow* window):
    _window(window)
{
    mice.emplace(_window, this);

    glfwSetCursorPosCallback(_window, cursor_position_callback);
    glfwSetScrollCallback(_window, scroll_callback);
}

Mouse::~Mouse()
{
    glfwSetCursorPosCallback(_window, nullptr);
    glfwSetCursorEnterCallback(_window, nullptr);
    glfwSetMouseButtonCallback(_window, nullptr);
    glfwSetScrollCallback(_window, nullptr);

    mice.erase(_window);
}

Mouse::MousePos Mouse::Pos()
{
    double x, y;
    glfwGetCursorPos(_window, &x, &y);

    return { x, y };
}

Mouse& Mouse::RegisterPosCallback(PosCallback cb, void* caller)
{
    _posCallbacks.emplace_back(cb, caller);
    return *this;
}

Mouse& Mouse::RegisterMoveCallback(PosCallback cb, void* caller)
{
    _moveCallbacks.emplace_back(cb, caller);
    return *this;
}

bool Mouse::Entered()
{
    return glfwGetWindowAttrib(_window, GLFW_HOVERED);
}

Mouse& Mouse::RegisterEnterCallback(EnterCallback cb, void* caller)
{
    _enterCallbacks.emplace_back(cb, caller);
    return *this;
}

Mouse::MousePress Mouse::Button(const MouseButton& button)
{
    return glfwGetMouseButton(_window, button);
}

Mouse& Mouse::RegisterButtonCallback(ButtonCallback cb, void* caller)
{
    _buttonCallbacks.emplace_back(cb, caller);
    return *this;
}

Mouse& Mouse::RegisterScrollCallback(ScrollCallback cb, void* caller)
{
    _scrollCallbacks.emplace_back(cb, caller);
    return *this;
}

void Mouse::callPosCallbacks(const double& x, const double& y)
{
    int width, height;
    glfwGetWindowSize(_window, &width, &height);

    double x_ = (2 * x / width) - 1;
    double y_ = 1 - (2 * y / height);

    for (auto& entry : _posCallbacks)
    {
        entry.cb(entry.caller, x_, y_);
    }

    double dx, dy;

    if (_prevInit)
    {
        dx = x_ - _prev.x;
        dy = y_ - _prev.y;
    }
    else
    {
        dx = 0;
        dy = 0;
        _prevInit = true;
    }

    _prev = { x_, y_ };

    for (auto& entry : _moveCallbacks)
    {
        entry.cb(entry.caller, dx, dy);
    }
}

void Mouse::callEnterCallbacks(const bool& enter)
{
    for (auto& entry : _enterCallbacks)
    {
        entry.cb(entry.caller, enter);
    }
}

void Mouse::callButtonCallbacks(const int& button, const int& action, const int& mod)
{
    for (auto& entry : _buttonCallbacks)
    {
        entry.cb(entry.caller, button, action, mod);
    }
}

void Mouse::callScrollCallbacks(const double& x, const double& y)
{
    for (auto& entry : _scrollCallbacks)
    {
        entry.cb(entry.caller, x, y);
    }
}
