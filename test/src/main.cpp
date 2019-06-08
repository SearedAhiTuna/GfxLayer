
#include <atomic>
#include <cstring>
#include <iostream>
#include <thread>

#include "Libs.h"
#include "Controller.h"
#include "TFShape.h"

Controller c;
TFShape* t;
Window* w;

void bgThreadCB();
std::atomic<bool> bgThreadDone;

int main(void)
{
	// Create a window
	w = c.createWindow();

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
	t->setProgramIndex(0);

	// Add the triangle to the window
	w->getRenderer().addShape(t);

	while (!bgThreadDone.load())
	{
		// Rotate the triangle
		t->rotExt(RELATIVE, PI / 50.0f);

		// Mark the window to be updated
		c.updateWindow(w);

		// Sleep the thread for a bit
		sleepMS(20);
	}
}