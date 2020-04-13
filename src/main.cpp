
#ifndef IS_LIBRARY

#include "Font.h"
#include "FrameCounter.h"
#include "Graphics.h"
#include "InternalLibs.h"
#include "Types.h"
#include "Mesh.h"
#include "Rectangle.h"
#include "Shape.h"
#include "Text.h"
#include "Window.h"

#include <atomic>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

void bgThread(Window* w);

int main(void)
{
    initGraphics(100);

    // Create a window
    getGraphics().createWindow(1028, 1028, "Test", bgThread);

    // Loop
    getGraphics().loop();

    return 0;
}

void rotate(void* w_, double dx, double dy)
{
    Window* w = reinterpret_cast<Window*>(w_);
    if (w->Mouse().Button(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        w->Camera().rot(true, PI * (GLfloat)dx, VEC3_DOWN);
        w->Camera().rot(true, PI * (GLfloat)dy, VEC3_RIGHT);
    }
}

void bgThread(Window* w)
{
    // Add shaders
    w->AddProgram("shaders/simple.vs", "shaders/simple.fs");

    Mesh m(Mesh::NORMALS_PER_FACE);
    int a = m.EmplaceVert(-.5, -.5, 0);
    int b = m.EmplaceVert(.5, -.5, 0);
    m.ExtrudeVerts({ a, b }, glm::translate(VEC3_UP));
    std::cout << m << "\n";

    Shape s;
    m.ToShape(s, 0, 1, 2);
    w->RegisterShape(s);

    // Move the camera a bit
    w->Camera().follow(true, 1.0f);

    // Set the perspective
    w->Camera().perspective(90.0f, 1.0f, .1f, 100.0f);

    // Set a callback when scrolling
    w->Mouse().RegisterMoveCallback(rotate, w);

    // Set the FPS to 60
    FrameCounter fps(60);

    while (!w->ShouldClose())
    {
        // Move the camera
        

        // Mark the window to be updated
        w->MarkForUpdate();

        // Wait for the next frame
        fps.wait();

        // Display FPS count
        //std::cout << "FPS=" << fps.fps() << std::endl;
    }
}

#endif