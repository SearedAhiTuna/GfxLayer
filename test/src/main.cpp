

#include "Arc.h"
#include "function/Bezier.h"
#include "FrameCounter.h"
#include "Graphics.h"
#include "Libs.h"
#include "Model.h"
#include "function/Parabola3D.h"
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

void rotate(void* w_, double x, double y)
{
    Window* w = reinterpret_cast<Window*>(w_);
    if (w->Mouse().Button(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        w->Camera().rot(false, PI * (GLfloat)y, VEC3_UP);
}

void bgThread(Window* w)
{
    // Add shaders
    w->AddProgram("shaders/simple.vs", "shaders/simple.fs");

    // Create curves
    auto* c1 = new Parabola3D(1, .5f, VEC3_ORIGIN);
    auto* c2 = new Parabola3D(1, .5f, VEC3_ORIGIN, VEC3_DOWN * (PI/2));
    auto* c3 = new Bezier<vec3>({ VEC3_RIGHT, VEC3_RIGHT + VEC3_BACKWARDS, VEC3_BACKWARDS });
    //bez->reserve(4);
    //bez->push_back(VEC3_RIGHT);
    //bez->push_back(VEC3_UP);
    //bez->push_back(VEC3_DOWN);
    //bez->push_back(VEC3_LEFT);

    // Create arcs
    Arc arc1(c1, 0, PI/2);
    Arc arc2(c2, 0, PI/2);
    Arc arc3(c3, 0, 1);

    // Create a model from the arc
    Model m;

    std::list<Model::Edge*> e1;
    arc1.generate(m, 5, e1);

    std::list<Model::Edge*> e2;
    arc2.generate(m, 5, e2);

    std::list<Model::Face*> fs;
    arc3.connect_edges(m, e1, e2, 5, fs);

    m.generate_face_normals(1);

    //m.edge_tf_3d(e2, MDL_ATT_POSITION, rotate(PI/4, VEC3_RIGHT));

    //std::cout << "Mesh:\n";
    //std::cout.flush();
    //m.print_verbose(std::cout);
    m.export_obj(std::cout, .01f);

    // Create a triangle
    Shape s;
    m.generate_shape(s);
    s.Program(0);
    //s.Texture("image.png");

    // Add the triangle to the window
    w->RegisterShape(s);

    // Move the camera a bit
    //w->getRenderer().getCamera().tl(true, VEC3_BACKWARDS * 1.0f);
    //w->getRenderer().getCamera().lookAt(VEC3_ORIGIN, false);
    w->Camera().follow(true, 1.0f);
    //w->Camera().follow(true, VEC3_UP * .5f);
    //w->Camera().rot(true, PI / 4, VEC3_LEFT);

    // Set the perspective
    w->Camera().perspective(90.0f, 1.0f, .1f, 100.0f);

    // Set a callback when scrolling
    w->Mouse().RegisterPosCallback(rotate, w);

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