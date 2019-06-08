
#include <atomic>
#include <cstring>
#include <iostream>
#include <thread>

#include "Libs.h"
#include "Camera.h"
#include "Controller.h"
#include "Renderer.h"
#include "TFShape.h"
#include "Window.h"

Controller c;
TFShape* t;
Window* w;

void bgThreadCB();
std::atomic<bool> bgThreadDone;

int main(void)
{
	// Create a window
	w = &c.createWindow(1028, 1028, "Test");

	// Add shaders
	w->getRenderer().addProgram("shaders/simple.vs", "shaders/simple.fs");

	// Create background thread
	std::thread bgThread(bgThreadCB);

	// Loop
	c.loop();

	// End the background thread
	bgThreadDone.store(true);
	bgThread.join();

	return 0;
}

void bgThreadCB()
{
	// Create a triangle
	t = new TFShape({ vec2(-0.1f,0.0f), vec2(0.1f,0.0f), vec2(0.0f,1.0f) });
	t->bufferCreate<vec3>({ vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) });
	t->setProgramIndex(0);

	// Add the triangle to the window
	w->getRenderer().addShape(*t);

	// Move the camera a bit
	//w->getRenderer().getCamera().tl(true, VEC3_BACKWARDS * 1.0f);
	//w->getRenderer().getCamera().lookAt(VEC3_ORIGIN, false);
	w->getRenderer().getCamera().follow(true, 2.0f);
	//w->getRenderer().getCamera().follow(true, VEC3_UP * .5f);

	// Set the perspective
	w->getRenderer().getCamera().perspective(60.0f, 1.0f, .1f, 100.0f);

	while (!bgThreadDone.load())
	{
		// Rotate the triangle
		t->rotExt(true, PI / 50.0f);

		// Move the camera
		w->getRenderer().getCamera().rot(true, PI / 100.0f, VEC3_UP);

		// Mark the window to be updated
		c.updateWindow(*w);

		// Sleep the thread for a bit
		sleepMS(20);
	}
}