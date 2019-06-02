
#include <cstring>
#include <iostream>

#include "Libs.h"
#include "Controller.h"

Controller c;

int main(void)
{
	Window* w = c.createWindow();
	c.createWindow();

	c.updateWindow(w);

	c.loop();

	return 0;
}

