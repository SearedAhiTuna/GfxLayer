
#include <atomic>
#include <cstring>
#include <iostream>
#include <thread>

#include "Camera.h"
#include "Graphics.h"
#include "Libs.h"
#include "Model.h"
#include "Renderer.h"
#include "TFShape.h"
#include "Window.h"

TFShape* t;
Model m;

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
    w->getRenderer().addProgram("shaders/simple.vs", "shaders/simple.fs");

    // Create a model
    int v1 = m.vert_create(vec2(-0.1f, 0.0f), UV_BOTTOM_LEFT);
    int v2 = m.vert_create(vec2(0.1f, 0.0f), UV_BOTTOM_RIGHT);
    int v3 = m.vert_create(vec2(0.1f, 1.0f), UV_TOP_RIGHT);
    int v4 = m.vert_create(vec2(-0.1f, 1.0f), UV_TOP_LEFT);
    m.face_create_from_nodes({ v1, v2, v3, v4 });
    std::cout << m << std::endl;

    // Create a triangle from the model
    t = dynamic_cast<TFShape*>(m.genShape());
    t->setProgramIndex(0);
    t->setTexture("image.png");

    // Add the triangle to the window
    w->getRenderer().addShape(*t);

    // Move the camera a bit
    //w->getRenderer().getCamera().tl(true, VEC3_BACKWARDS * 1.0f);
    //w->getRenderer().getCamera().lookAt(VEC3_ORIGIN, false);
    w->getRenderer().getCamera().follow(true, 2.0f);
    w->getRenderer().getCamera().follow(true, VEC3_UP * .5f);

    // Set the perspective
    w->getRenderer().getCamera().perspective(60.0f, 1.0f, .1f, 100.0f);

    while (!w->shouldClose())
    {
        // Rotate the triangle
        //t->rotExt(true, PI / 50.0f);

        // Move the camera
        w->getRenderer().getCamera().rot(true, PI / 100.0f, VEC3_UP);

        // Mark the window to be updated
        w->markForUpdate();

        // Sleep the thread for a bit
        sleepMS(20);
    }
}