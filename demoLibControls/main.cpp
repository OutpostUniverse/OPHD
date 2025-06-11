#include "MainWindowState.h"

#include <NAS2D/Game.h>

#include <iostream>


int main()
{
	try
	{
		NAS2D::Game game{"Demo libControls", "DemoLibControls", "LairWorks"};
		game.go(new MainWindowState);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nException: " << exception.what() << std::endl;
	}

	return 0;
}
