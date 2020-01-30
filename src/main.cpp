// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "NAS2D/Resources/Image.h"
#include "NAS2D/Resources/Music.h"
#include "NAS2D/Mixer/MixerSDL.h"
#include "NAS2D/Mixer/MixerNull.h"
#include "NAS2D/Renderer/RendererOpenGL.h"

#include "Common.h"
#include "Constants.h"
#include "StructureCatalogue.h"
#include "StructureTranslator.h"
#include "WindowEventWrapper.h"

#include "Things/Structures/Structure.h"

#include "States/MapViewState.h"
#include "States/MainMenuState.h"
#include "States/SplashState.h"

#include "States/MainReportsUiState.h"

#include <iostream>
#include <fstream>

using namespace NAS2D;

#include "SDL.h"

/** Not thrilled with placement but this seems to be the easiest way to deal with it. */
NAS2D::Image* IMG_LOADING = nullptr;
NAS2D::Image* IMG_SAVING = nullptr;
NAS2D::Image* IMG_PROCESSING_TURN = nullptr;

NAS2D::Music* MARS = nullptr;


/**
 * Makes sure video resolution is never less than 1024x768
 */
void validateVideoResolution()
{
	Configuration& cf = Utility<Configuration>::get();

	if (cf.graphicsWidth() < constants::MINIMUM_WINDOW_WIDTH) { cf.graphicsWidth(constants::MINIMUM_WINDOW_WIDTH); }
	if (cf.graphicsHeight() < constants::MINIMUM_WINDOW_HEIGHT) { cf.graphicsHeight(constants::MINIMUM_WINDOW_HEIGHT); }
	cf.fullscreen(false); // force windowed mode.
}


int main(int argc, char *argv[])
{
	(void)argc; //UNUSED
	//Crude way of redirecting stream buffer when building in release (no console)
	#ifdef NDEBUG
	std::streambuf *backup;
	std::ofstream filestr;
	filestr.open("ophd.log");

	backup = std::cout.rdbuf();
	std::cout.rdbuf(filestr.rdbuf());
	#endif

	std::cout << "OutpostHD " << constants::VERSION << std::endl << std::endl;

	StructureCatalogue::init();		// only needs to be done once at the start of the program.
	StructureTranslator::init();	// only needs to be done once at the start of the program.

	try
	{
		Filesystem& f = Utility<Filesystem>::init<Filesystem>(argv[0], "OutpostHD", "LairWorks", "data");

		if (!f.exists(constants::SAVE_GAME_PATH))
		{
			f.makeDirectory(constants::SAVE_GAME_PATH);
		}

		Configuration& cf = Utility<Configuration>::get();

		// If no config file, set defaults.
		if (!f.exists("config.xml"))
		{
			cf.graphicsWidth(constants::MINIMUM_WINDOW_WIDTH);
			cf.graphicsHeight(constants::MINIMUM_WINDOW_HEIGHT);
			cf.fullscreen(false);
		}

		cf.load("config.xml");

		if (cf.option("skip-splash").empty())
		{
			cf.option("skip-splash", "false");
		}
		if (cf.option("maximized").empty())
		{
			cf.option("maximized", "true");
		}

		validateVideoResolution();

		try
		{
			Utility<Mixer>::init<MixerSDL>();
		}
		catch (...)
		{
			Utility<MixerNull>::init();
		}

		WindowEventWrapper _wew;

		Renderer& r = Utility<Renderer>::init<RendererOpenGL>("OutpostHD");

		std::cout << std::endl << "** GAME START **" << std::endl << std::endl;

		r.minimum_size(1000, 700);
		r.resizeable(true);
		r.addCursor(constants::MOUSE_POINTER_NORMAL, POINTER_NORMAL, 0, 0);
		r.addCursor(constants::MOUSE_POINTER_PLACE_TILE, POINTER_PLACE_TILE, 16, 16);
		r.addCursor(constants::MOUSE_POINTER_INSPECT, POINTER_INSPECT, 8, 8);
		r.setCursor(POINTER_NORMAL);
		r.fadeOut(0.0f);

		if (cf.option("maximized") == "true")
		{
			/** \fixme Evil hack exposing an internal NAS2D variable. */
			extern SDL_Window* underlyingWindow;
			SDL_MaximizeWindow(underlyingWindow);
		}

		std::cout << "Loading packed assets... ";

		//f.mount("fonts.dat");
		//f.mount("planets.dat");

		std::cout << "done." << std::endl;

		// Loading/Saving plaque's
		IMG_LOADING = new Image("sys/loading.png");
		IMG_SAVING = new Image("sys/saving.png");
		IMG_PROCESSING_TURN = new Image("sys/processing_turn.png");

		MARS = new Music("music/mars.ogg");
		Utility<Mixer>::get().playMusic(*MARS);

		StateManager stateManager;
		stateManager.forceStopAudio(false);
		
		if (cf.option("skip-splash") == "false")
		{
			stateManager.setState(new SplashState());
		}
		else
		{
			stateManager.setState(new MainMenuState());
		}

		// Game Loop
		while (stateManager.update())
		{
			Utility<Renderer>::get().update();
		}

		cf.save(); // force configuration to save any changes.
	}
	catch(const std::exception& e)
	{
		doNonFatalErrorMessage("Application Error", e.what());
	}

	#ifdef NDEBUG
	filestr.close();
	#endif

	SDL_Quit();

	return 0;
}
