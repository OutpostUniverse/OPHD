#include "Cache.h"
#include "Constants/Strings.h"
#include "Constants/Numbers.h"
#include "WindowEventWrapper.h"
#include "PointerType.h"

#include "States/GameState.h"
#include "States/SplashState.h"
#include "States/MainMenuState.h"
#include "States/MapViewState.h"
#include "States/MainReportsUiState.h"

#include "UI/MessageBox.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/EventHandler.h>
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


namespace {
	void dumpGraphicsInfo(RendererOpenGL& renderer)
	{
		std::vector<std::string> info{
			"- OpenGL System Info -",
			"Vendor: " + renderer.getVendor(),
			"Renderer: " + renderer.getRenderer(),
			"Driver Version: " + renderer.getDriverVersion(),
			"GLSL Version: " + renderer.getShaderVersion(),
		};

		for (const auto& str : info)
		{
			std::cout << "\t" << str << std::endl;
		}
	}


	/**
	 * Mount data folder(s) for static assets
	 *
	 * Prioritize data from working directory, fallback on data from executable path,
	 * searching up to 2 levels above the executable
	 *
	 * @return Nonzero on success, zero on error.
	 */
	int mountDataFolder(Filesystem& filesystem)
	{
		// Current working directory may contain a partial data folder with customizations
		int result = filesystem.mountSoftFail("data");

		// Assuming above folder was partial, we still want full data loaded for static assets
		// We find static assets folder with executable or in parent folders
		auto basePath = filesystem.basePath();
		for (int i = 0; i <= 2; ++i)
		{
			result = filesystem.mountSoftFail(basePath / "data");
			if (result != 0)
			{
				return result;
			}
			basePath = basePath / "..";
		}
		return result;
	}
}


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

	std::cout << "OutpostHD " << constants::Version << std::endl << std::endl;

	try
	{
		auto& filesystem = Utility<Filesystem>::init<Filesystem>("OutpostHD", "LairWorks");
		mountDataFolder(filesystem);
		filesystem.mountReadWrite(filesystem.prefPath());

		filesystem.makeDirectory(constants::SaveGamePath);

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
		std::cout << "Initializing Configuration... ";
		cf.load("config.xml");
		std::cout << "done." << std::endl;

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

		WindowEventWrapper windowEventWrapper;

		std::cout << "Starting OpenGL Renderer:" << std::endl;
		auto& renderer = Utility<Renderer>::init<RendererOpenGL>("OutpostHD");

		dumpGraphicsInfo(renderer);
		std::cout << std::endl << "** GAME START **" << std::endl << std::endl;

		renderer.minimumSize(constants::MinimumWindowSize);
		renderer.resizeable(true);

		addCursor(PointerType::Normal, constants::MousePointerNormal, {0, 0});
		addCursor(PointerType::PlaceTile, constants::MousePointerPlaceTile, {16, 16});
		setCursor(PointerType::Normal);

		Control::setDefaultFont(fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal));
		Control::setDefaultFontBold(fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryNormal));
		Control::setImageCache(imageCache);

		const auto& options = cf["options"];
		if (options.get<bool>("maximized"))
		{
			renderer.maximize();
		}

		trackMars = std::make_unique<NAS2D::Music>("music/mars.ogg");
		Utility<Mixer>::get().playMusic(*trackMars);

		StateManager stateManager;
		stateManager.forceStopAudio(false);

		if (argc > 1)
		{
			std::string filename = constants::SaveGamePath + argv[1] + ".xml";
			if (!filesystem.exists(filename))
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
			renderer.update();
		}

		cf.save("config.xml"); // force configuration to save any changes.
	}
	catch(const std::exception& e)
	{
		doNonFatalErrorMessage("Application Error", e.what());
	}

	imageCache.clear();
	Utility<Renderer>::clear();
	std::cout << "OpenGL Renderer Terminated." << std::endl;
	Utility<EventHandler>::clear();
	std::cout << "EventHandler Terminated." << std::endl;
	Utility<Mixer>::clear();
	Utility<Configuration>::clear();
	Utility<Filesystem>::clear();

	#ifdef NDEBUG
	// Reset to stdout again (prevents crashes on exit)
	std::cout.rdbuf(originalCoutBuffer);
	#endif

	SDL_Quit();

	return 0;
}
