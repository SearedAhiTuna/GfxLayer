
#pragma once

#include "Types.h"

#include <list>

class Mouse final
{
public:
    struct MousePos
    {
        double x;
        double y;
    };

    typedef int MouseButton;
    typedef int MousePress;
    typedef int MouseMod;

    struct MouseScroll
    {
        double x;
        double y;
    };
    
    typedef void (*PosCallback)(void*, double, double);
    typedef void (*EnterCallback)(void*, bool);
    typedef void (*ButtonCallback)(void*, MouseButton, MousePress, MouseMod);
    typedef void (*ScrollCallback)(void*, double, double);

public:
    Mouse(GLFWwindow* window);
    ~Mouse();

    MousePos Pos();
    Mouse& RegisterPosCallback(PosCallback cb, void* caller = nullptr);
    Mouse& RegisterMoveCallback(PosCallback cb, void* caller = nullptr);

    bool Entered();
    Mouse& RegisterEnterCallback(EnterCallback cb, void* caller = nullptr);

    MousePress Button(const MouseButton& button);
    Mouse& RegisterButtonCallback(ButtonCallback cb, void* caller = nullptr);

    Mouse& RegisterScrollCallback(ScrollCallback cb, void* caller = nullptr);

private:
    void callPosCallbacks(const double& x, const double& y);
    void callEnterCallbacks(const bool& enter);
    void callButtonCallbacks(const int& button, const int& action, const int& mod);
    void callScrollCallbacks(const double& x, const double& y);

private:
    friend class Window;

    friend void cursor_position_callback(GLFWwindow* window, double x, double y);
    friend void cursor_enter_callback(GLFWwindow* window, int entered);
    friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
    template <typename Callback>
    struct Entry
    {
        Entry(Callback cb_, void* caller_):
            cb(cb_),
            caller(caller_)
        {
        }

        Callback cb;
        void* caller;
    };

private:
    GLFWwindow* _window;

    std::list<Entry<PosCallback>> _posCallbacks;
    std::list<Entry<EnterCallback>> _enterCallbacks;
    std::list<Entry<ButtonCallback>> _buttonCallbacks;
    std::list<Entry<ScrollCallback>> _scrollCallbacks;

    bool _prevInit{};
    MousePos _prev;
    std::list<Entry<PosCallback>> _moveCallbacks;

};

#define MOUSE_POS_CALLBACK(CLASS, FUNC, CBFUNC) \
void CBFUNC(void* caller_, double x, double y) \
{ \
    CLASS* caller = reinterpret_cast<CLASS*>(caller_); \
    caller->FUNC(x, y); \
}

#define MOUSE_ENTER_CALLBACK(CLASS, FUNC, CBFUNC) \
void CBFUNC(void* caller_, bool enter) \
{ \
    CLASS* caller = reinterpret_cast<CLASS*>(caller_); \
    caller->FUNC(enter); \
}

#define MOUSE_BUTTON_CALLBACK(CLASS, FUNC, CBFUNC) \
void CBFUNC(void* caller_, int button, int action, int mod) \
{ \
    CLASS* caller = reinterpret_cast<CLASS*>(caller_); \
    caller->FUNC(button, action, mod); \
}

#define MOUSE_SCROLL_CALLBACK(CLASS, FUNC, CBFUNC) \
void CBFUNC(void* caller_, double x, double y) \
{ \
    CLASS* caller = reinterpret_cast<CLASS*>(caller_); \
    caller->FUNC(x, y); \
}