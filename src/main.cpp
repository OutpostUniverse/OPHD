#include "NAS2D/NAS2D.h"

#include "NAS2D/Mixer/Mixer_SDL.h"
#include "NAS2D/Renderer/OGL_Renderer.h"

#include "States/GameState.h"
#include "States/PlanetSelectState.h"
#include "States/SplashState.h"

#include <iostream>
#include <fstream>

using namespace NAS2D;
using namespace std;

#include "SDL.h"

void createRenderer()
{
	Utility<Renderer>::instantiateDerived(new OGL_Renderer("OutpostHD"));
}

// Makes sure video resolution is never less than 800x600
void validateVideoResolution()
{
	Configuration& cf = Utility<Configuration>::get();

	if (cf.graphicsWidth() < 800 || cf.graphicsHeight() < 600)
	{
		cf.graphicsWidth(800);
		cf.graphicsHeight(600);
	}
}


int main(int argc, char *argv[])
{
	//Crude way of redirecting stream buffer when building in release (no console)
	#ifdef NDEBUG
	std::streambuf *backup;
	std::ofstream filestr;
	filestr.open("ophd.log");

	backup = std::cout.rdbuf();
	std::cout.rdbuf(filestr.rdbuf());
	#endif

	try
	{
		Filesystem& f = Utility<Filesystem>::get();
		f.init(argv[0], "data");

		Utility<EventHandler>::get();

		Configuration& cf = Utility<Configuration>::get();
		
		// If no config file, set defaults.
		if (!f.exists("config.xml"))
		{
			// FIXME: Hacky as hell!
			SDL_Init(SDL_INIT_VIDEO);
			SDL_DisplayMode dm;
			if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
			{
				cout << "SDL_GetDesktopDisplayMode failed: " << SDL_GetError();
				return 1;
			}

			cf.graphicsWidth(dm.w);
			cf.graphicsHeight(dm.h);
			cf.fullscreen(true);
		}
					
		cf.load("config.xml");

		if (cf.option("skip-splash").empty())
			cf.option("skip-splash", "false");

		validateVideoResolution();

		cf.save();

		try
		{
			Utility<Mixer>::instantiateDerived(new Mixer_SDL());
		}
		catch (...)
		{
			Utility<Mixer>::instantiateDerived(new Mixer());
		}

		createRenderer();

		f.addToSearchPath("fonts.dat");
		f.addToSearchPath("maps.dat");
		f.addToSearchPath("planets.dat");
		f.addToSearchPath("robots.dat");
		f.addToSearchPath("sfx.dat");
		f.addToSearchPath("structures.dat");
		f.addToSearchPath("sys.dat");
		f.addToSearchPath("ui.dat");
		
		if (!f.exists(constants::SAVE_GAME_PATH))
			f.makeDirectory(constants::SAVE_GAME_PATH);

		StateManager stateManager;

		/*
		if(cf.option("skip-splash") == "false")
			stateManager.setState(new SplashState());
		else*/
			stateManager.setState(new PlanetSelectState());
		


		// Game Loop
		while (stateManager.update())
			Utility<Renderer>::get().update();

	}
	catch(const std::exception& e)
	{
		cout << e.what() << endl;
	}

#ifdef NDEBUG
	filestr.close();
#endif

	SDL_Quit();

	return 0;
}
