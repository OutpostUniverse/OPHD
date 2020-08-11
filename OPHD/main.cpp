// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Cache.h"
#include "Common.h"
#include "Constants.h"
#include "StructureTranslator.h"
#include "WindowEventWrapper.h"

#include "States/SplashState.h"
#include "States/MainMenuState.h"
#include "States/MapViewState.h"
#include "States/MainReportsUiState.h"

#include <NAS2D/Resources/Image.h>
#include <NAS2D/Resources/Music.h>
#include <NAS2D/Mixer/MixerSDL.h>
#include <NAS2D/Mixer/MixerNull.h>
#include <NAS2D/Renderer/RendererOpenGL.h>
#include <NAS2D/StateManager.h>

#include <SDL2/SDL.h>

#include <iostream>
#include <fstream>


using namespace NAS2D;


/** Not thrilled with placement but this seems to be the easiest way to deal with it. */
const NAS2D::Image* IMG_LOADING = nullptr;
const NAS2D::Image* IMG_SAVING = nullptr;
const NAS2D::Image* IMG_PROCESSING_TURN = nullptr;


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


int main(int /*argc*/, char *argv[])
{
	//Crude way of redirecting stream buffer when building in release (no console)
	#ifdef NDEBUG
	std::streambuf *backup;
	std::ofstream filestr;
	filestr.open("ophd.log");

	backup = std::cout.rdbuf();
	std::cout.rdbuf(filestr.rdbuf());
	#endif

	std::cout << "OutpostHD " << constants::VERSION << std::endl << std::endl;

	StructureTranslator::init(); // only needs to be done once at the start of the program.

	try
	{
		auto& fs = Utility<Filesystem>::init<Filesystem>(argv[0], "OutpostHD", "LairWorks");
		// Prioritize data from working directory, fallback on data from executable path
		fs.mountSoftFail("data");
		fs.mountSoftFail(fs.basePath() + "data");
		fs.mountReadWrite(fs.prefPath());

		fs.makeDirectory(constants::SAVE_GAME_PATH);

		Configuration& cf = Utility<Configuration>::init(
			std::map<std::string, Dictionary>{
				{
					"graphics",
					{{
						{"screenwidth", constants::MINIMUM_WINDOW_WIDTH},
						{"screenheight", constants::MINIMUM_WINDOW_HEIGHT},
						{"bitdepth", 32},
						{"fullscreen", false},
						{"vsync", true}
					}}
				},
				{
					"audio",
					{{
						{"mixer", "SDL"},
						{"musicvolume", 100},
						{"sfxvolume", 128},
						{"channels", 2},
						{"mixrate", 22050},
						{"bufferlength", 1024}
					}}
				},
				{
					"options",
					{{
						{"skip-splash", false},
						{"maximized", true}
					}}
				}
			}
		);
		cf.load("config.xml");
		validateVideoResolution();

		try
		{
			Utility<Mixer>::init<MixerSDL>();
		}
		catch (...)
		{
			Utility<Mixer>::init<MixerNull>();
		}

		WindowEventWrapper _wew;

		auto& renderer = Utility<Renderer>::init<RendererOpenGL>("OutpostHD");

		std::cout << std::endl << "** GAME START **" << std::endl << std::endl;

		renderer.minimumSize({1000, 700});
		renderer.resizeable(true);
		renderer.addCursor(constants::MOUSE_POINTER_NORMAL, PointerType::POINTER_NORMAL, 0, 0);
		renderer.addCursor(constants::MOUSE_POINTER_PLACE_TILE, PointerType::POINTER_PLACE_TILE, 16, 16);
		renderer.addCursor(constants::MOUSE_POINTER_INSPECT, PointerType::POINTER_INSPECT, 8, 8);
		renderer.setCursor(PointerType::POINTER_NORMAL);

		if (cf.option("maximized") == "true")
		{
			/** \fixme Evil hack exposing an internal NAS2D variable. */
			extern SDL_Window* underlyingWindow;
			SDL_MaximizeWindow(underlyingWindow);
		}

		std::cout << "Loading packed assets... ";

		fs.mountSoftFail("fonts.dat");
		fs.mountSoftFail("planets.dat");

		std::cout << "done." << std::endl;

		// Loading/Saving plaque's
		IMG_LOADING = &imageCache.load("sys/loading.png");
		IMG_SAVING = &imageCache.load("sys/saving.png");
		IMG_PROCESSING_TURN = &imageCache.load("sys/processing_turn.png");

		trackMars = std::make_unique<NAS2D::Music>("music/mars.ogg");
		Utility<Mixer>::get().playMusic(*trackMars);

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

		cf.save("config.xml"); // force configuration to save any changes.
	}
	catch(const std::exception& e)
	{
		doNonFatalErrorMessage("Application Error", e.what());
	}

	imageCache.clear();

	#ifdef NDEBUG
	filestr.close();
	#endif

	SDL_Quit();

	return 0;
}
