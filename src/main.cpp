
#ifndef IS_LIBRARY

#include "Arc.h"
#include "function/Bezier.h"
#include "Font.h"
#include "FrameCounter.h"
#include "Graphics.h"
#include "InternalLibs.h"
#include "Types.h"
#include "Model.h"
#include "function/Parabola3D.h"
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

    // Create a model
    Model m;
    Model m1;
    Model m2;

    //m.verts.emplace(VEC3_ORIGIN, UV_BOTTOM_LEFT);
    //m.verts.emplace(VEC3_RIGHT, UV_BOTTOM_RIGHT);
    //m.verts.emplace(VEC3_RIGHT + VEC3_UP, UV_TOP_RIGHT);
    //m.verts.emplace(VEC3_UP, UV_TOP_LEFT);

    //m.faces.emplace_verts(Verts(m.verts(0), m.verts(1), m.verts(2), m.verts(3)));

    Arc* a1 = new Arc(new Parabola3D(.5f, .5f, VEC3_ORIGIN, PI/2*VEC3_UP), PI, PI / 2);
    std::list<Model::Edge*> edges1;
    a1->generate(m1, 10, edges1);

    Arc* a2 = new Arc(new Parabola3D(.25f, .35f, .25f * VEC3_RIGHT, PI / 2 * VEC3_UP), PI, PI/2);
    std::list<Model::Edge*> edges2;
    a2->generate(m1, 10, edges2);
    std::list<Model::Edge*> edges3;
    a2->generate(m2, 10, edges3);

    Model::Vert& v = m2.verts.emplace(.5f * VEC3_RIGHT);

    Arc* a3 = new Arc(new Parabola3D(1, 1, VEC3_ORIGIN, PI/2 * VEC3_RIGHT), 0, PI / 2);
    std::list<Model::Face*> faces;
    a3->connect_edges(m1, edges2, edges1, 10, faces);
    a3->connect_fan(m2, v, edges3, 10, faces);

    m += m1;
    m += m2;

    m.generate_face_normals();
    //m2.generate_face_normals();

    //std::cout << "m1=\n";
    //std::cout << m1 << std::endl;
    //std::cout.flush();

    //std::cout << "m2=\n";
    //std::cout << m2 << std::endl;
    //std::cout.flush();

    //std::cout << "m=\n";
    //std::cout << m << std::endl;
    //std::cout.flush();

    // Create a shape from the model
    Shape s;
    m.generate_shape(s);
    s.Program(0);
    //s.TF(false, translate(VEC3_FORWARDS));
    s.Texture("image.png");

    //std::cout << "Shape:\n" << s << std::endl;
    //std::cout.flush();

    w->RegisterShape(s);

    //m.edge_tf_3d(e2, MDL_ATT_POSITION, rotate(PI/4, VEC3_RIGHT));

    //std::cout << "Mesh:\n";
    //std::cout.flush();
    //m.print_verbose(std::cout);
    //m.export_obj(std::cout, .01f);
    Font f("C:/Windows/Fonts/times.ttf", 256, vec3(1,0,0));
    
    Text t(f, VEC3_RIGHT * .2f, VEC3_UP * .2f, "Heyo");
    t.Program(0);

    // Add the text
    //w->RegisterShapes(t);

    // Move the camera a bit
    //w->getRenderer().getCamera().tl(true, VEC3_BACKWARDS * 1.0f);
    //w->getRenderer().getCamera().lookAt(VEC3_ORIGIN, false);
    w->Camera().follow(true, 1.0f);
    //w->Camera().follow(true, VEC3_UP * .5f);
    //w->Camera().rot(true, PI / 4, VEC3_LEFT);

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