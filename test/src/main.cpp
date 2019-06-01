
#include <iostream>

#include "Graphics.h"

Graphics g;

int main(void)
{
	ScreenID scr1 = g.createWindow(1000,1000,"Yo");
	ScreenID scr2 = g.createWindow();

	g.setClearColor(vec4(0.0f, 0.0f, 0.4f, 1.0f));
	g.redraw();

	g.loop();
}

