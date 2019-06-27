

#include "Camera.h"
#include "Graphics.h"
#include "Libs.h"
#include "Model.h"
#include "Shape.h"
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

void bgThread(Window* w)
{
    // Add shaders
    w->AddProgram("shaders/simple.vs", "shaders/simple.fs");

    // Create a model
    Model m;
    m.verts().emplace(VEC3_ORIGIN, UV_BOTTOM_LEFT);
    m.verts().emplace(VEC3_UP, UV_TOP_LEFT);
    m.verts().emplace(VEC3_UP + VEC3_RIGHT, UV_TOP_RIGHT);
    m.verts().emplace(VEC3_RIGHT, UV_BOTTOM_RIGHT);
    auto f = m.faces().emplace_verts(Verts(&m.vert(0), &m.vert(1), &m.vert(2), &m.vert(3)));

    auto f2 = f.extrude();
    m.face_tf_3d(f2, MDL_ATT_POSITION, glm::translate(glm::mat4(), VEC3_FORWARDS * .5f));

    std::cout << "Mesh:\n";
    std::cout.flush();
    m.print_verbose(std::cout);

    // Create a triangle
    Shape s;
    m.generate_shape(s);
    s.Program(0);
    s.Texture("image.png");

    // Add the triangle to the window
    w->RegisterShape(s);

    // Move the camera a bit
    //w->getRenderer().getCamera().tl(true, VEC3_BACKWARDS * 1.0f);
    //w->getRenderer().getCamera().lookAt(VEC3_ORIGIN, false);
    w->Camera().follow(true, 2.0f);
    w->Camera().follow(true, VEC3_UP * .5f);
    w->Camera().rot(true, PI / 4, VEC3_LEFT);

    // Set the perspective
    w->Camera().perspective(60.0f, 1.0f, .1f, 100.0f);

    while (!w->ShouldClose())
    {
        // Move the camera
        w->Camera().rot(true, PI / 100.0f, VEC3_UP);

        // Mark the window to be updated
        w->MarkForUpdate();

        // Sleep the thread for a bit
        sleepMS(20);
    }
}