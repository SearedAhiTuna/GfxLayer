
#include "Window.h"
#include "Camera.h"
#include "Graphics.h"
#include "InternalLibs.h"
#include "Mouse.h"
#include "Program.h"
#include "Shape.h"

#include <iostream>

Window::Window():
    _window(nullptr),
    _vao(0),
    _camera(nullptr),
    _mouse(nullptr)
{
}

Window::Window(GLFWwindow* window) :
    _window(window),
    _vao(0)
{
    assertMainThread();

    // Generate a vertex array object
    GLuint temp = 0;
    glGenVertexArrays(1, &temp);
    _vao = temp;

    _camera = std::make_unique<::Camera>();

    _mouse = std::make_unique<::Mouse>(_window);
}

Window::Window(Window&& other) noexcept :
    _window(other._window),
    _vao(other._vao)
{
    std::lock_guard<std::mutex> otherLk(other._shapeLock);
    for (Shape* s : other._shapes)
    {
        _shapes.emplace(s);
    }

    other._window = nullptr;
    other._vao = 0;

    other._shapes.clear();
}

Window::~Window()
{
    Free();
}

Window& Window::operator=(Window&& rhs) noexcept
{
    Free();

    _window = rhs._window;
    rhs._window = nullptr;

    _vao = rhs._vao;
    rhs._vao = 0;

    std::lock_guard<std::mutex> myLk(_shapeLock);
    std::lock_guard<std::mutex> otherLk(rhs._shapeLock);
    for (Shape* s : rhs._shapes)
    {
        _shapes.emplace(s);
    }

    return *this;
}

void Window::Free()
{
    if (_window)
    {
        assertMainThread();
        glfwDestroyWindow(_window);
        _window = nullptr;
    }

    if (_vao)
    {
        assertMainThread();
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }

    std::lock_guard<std::mutex> lk(_shapeLock);
    _shapes.clear();
}

void Window::Update()
{
    assertMainThread();

    // Set window as current context
    glfwMakeContextCurrent(_window);

    // Clear the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the VAO
    glBindVertexArray(_vao);

    // Draw all shapes
    std::lock_guard<std::mutex> lk(_shapeLock);
    for (Shape* s : _shapes)
    {
        Draw(*s);
    }

    // Swap the buffers
    glfwSwapBuffers(_window);

    // Mark as updated
    _updated.store(true);
}

void Window::Draw(Shape& s)
{
    Program* p;

    if (s.UsesProgram() && (p = UseProgram(s.Program())))
    {
        mat4 mvpMat{};
        mvpMat = s.TF() * mvpMat;
        mvpMat = _camera->getV() * mvpMat;
        mvpMat = _camera->getP() * mvpMat;
        p->SetMVP(mvpMat);

        if (s.Texture())
        {
            p->SetTexture(s.Texture());
        }
    }
    else
    {
        UseProgram(-1);
    }

    // Update the shape
    s.UpdateBuffers();

    // Lock the shape
    std::lock_guard<std::mutex> bLk(s._bigLock);

    // For each buffer
    GLuint index = 0;
    for (auto& ptr : s._buffers)
    {
        Shape::Buffer& buf = *ptr.get();

        // Enable this attribute array
        glEnableVertexAttribArray(index);

        // Bind the buffer
        glBindBuffer(GL_ARRAY_BUFFER, buf._vbo);

        // Set the attribute pointer
        glVertexAttribPointer(
            index,              // attribute.
            (GLint)buf._ndims,    // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // Increment index
        ++index;
    }

    // Draw the arrays
    glDrawArrays(s.Mode(), 0, (GLsizei)s.NumVerts());

    // Disable the attribute arrays
    for (GLuint i = 0; i < index; ++i)
    {
        glDisableVertexAttribArray(i);
    }
}

void Window::MarkForUpdate()
{
    _updated.store(false);
    glfwPostEmptyEvent();
}

bool Window::ShouldClose()
{
    if (onMainThread())
    {
        return glfwWindowShouldClose(_window);
    }
    else
    {
        return _extShouldClose.load();
    }
}

void Window::RegisterShape(Shape& s)
{
    std::lock_guard<std::mutex> lk(_shapeLock);
    _shapes.emplace(&s);
}

void Window::DeregisterShape(Shape& s)
{
    std::lock_guard<std::mutex> lk(_shapeLock);
    _shapes.erase(&s);
}

void Window::AddProgram(const std::string& vert, const std::string& frag)
{
    std::lock_guard<std::mutex> lk(_programLock);
    _programs.push_back(std::make_unique<Program>(vert, frag));
}

Program* Window::UseProgram(const int& index)
{
    std::lock_guard<std::mutex> lk(_programLock);

    if (index == -1 || index >= _programs.size())
    {
        if (index != _curProgram)
        {
            _curProgram = -1;
            glUseProgram(0);
        }

        return nullptr;
    }
    else
    {
        if (index != _curProgram)
        {
            _curProgram = index;
            glUseProgram(_programs[_curProgram]->ID());
        }

        return _programs[_curProgram].get();
    }
}