#include "Cache.h"
#include "Common.h"
#include "Constants.h"
#include "WindowEventWrapper.h"

#include "States/GameState.h"
#include "States/SplashState.h"
#include "States/MainMenuState.h"
#include "States/MapViewState.h"
#include "States/MainReportsUiState.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/StateManager.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Music.h>
#include <NAS2D/Mixer/MixerSDL.h>
#include <NAS2D/Mixer/MixerNull.h>
#include <NAS2D/Renderer/RendererOpenGL.h>

#include <SDL2/SDL.h>

#include <iostream>
#include <fstream>


using namespace NAS2D;


int main(int argc, char *argv[])
{
	// Crude way of redirecting stream buffer when building in release (no console)
	#ifdef NDEBUG
	// Save for later
	std::streambuf* originalCoutBuffer = std::cout.rdbuf();
	// Redirect output to log file
	std::ofstream logFile("ophd.log");
	std::cout.rdbuf(logFile.rdbuf());
	#endif

	std::cout << "OutpostHD " << constants::VERSION << std::endl << std::endl;

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
						{"screenwidth", constants::MinimumWindowSize.x},
						{"screenheight", constants::MinimumWindowSize.y},
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

		// Ensure minimum video resolution
		auto& graphics = cf["graphics"];
		if (graphics.get<int>("screenwidth") < constants::MinimumWindowSize.x) { graphics.set("screenwidth", constants::MinimumWindowSize.x); }
		if (graphics.get<int>("screenheight") < constants::MinimumWindowSize.y) { graphics.set("screenheight", constants::MinimumWindowSize.y); }
		// Force windowed mode
		graphics.set("fullscreen", false);

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

		renderer.minimumSize(constants::MinimumWindowSize);
		renderer.resizeable(true);
		renderer.addCursor(constants::MOUSE_POINTER_NORMAL, PointerType::POINTER_NORMAL, 0, 0);
		renderer.addCursor(constants::MOUSE_POINTER_PLACE_TILE, PointerType::POINTER_PLACE_TILE, 16, 16);
		renderer.addCursor(constants::MOUSE_POINTER_INSPECT, PointerType::POINTER_INSPECT, 8, 8);
		renderer.setCursor(PointerType::POINTER_NORMAL);

		const auto& options = cf["options"];
		if (options.get<bool>("maximized"))
		{
			/** \fixme Evil hack exposing an internal NAS2D variable. */
			extern SDL_Window* underlyingWindow;
			SDL_MaximizeWindow(underlyingWindow);
		}

		trackMars = std::make_unique<NAS2D::Music>("music/mars.ogg");
		Utility<Mixer>::get().playMusic(*trackMars);

		StateManager stateManager;
		stateManager.forceStopAudio(false);

		if (argc > 1)
		{
			std::string filename = constants::SAVE_GAME_PATH + argv[1] + ".xml";
			if (!fs.exists(filename))
			{
				std::cout << "Savegame specified on command line: " << argv[1] << " could not be found." << std::endl;
				stateManager.setState(new MainMenuState());
			}

			Utility<Mixer>::get().stopMusic();

			GameState* gameState = new GameState();
			MapViewState* mapview = new MapViewState(gameState->getMainReportsState(), filename);
			mapview->_initialize();
			mapview->activate();

			gameState->mapviewstate(mapview);
			stateManager.setState(gameState);
		}
		else if (!options.get<bool>("skip-splash"))
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
	// Reset to stdout again (prevents crashes on exit)
	std::cout.rdbuf(originalCoutBuffer);
	#endif

	SDL_Quit();

	return 0;
}
