#include "Cache.h"
#include "MainWindowState.h"

#include <libControls/Control.h>

#include <NAS2D/Game.h>
#include <NAS2D/Resource/ResourceCache.h>

#include <iostream>


int main()
{
	try
	{
		NAS2D::Game game{"Demo libControls", "DemoLibControls", "LairWorks"};

		Control::setDefaultFont(fontCache.load("fonts/opensans.ttf", 14));
		Control::setDefaultFontBold(fontCache.load("fonts/opensans-bold.ttf", 14));
		Control::setImageCache(imageCache);

		game.go(new MainWindowState);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nException: " << exception.what() << std::endl;
	}

	return 0;
}
