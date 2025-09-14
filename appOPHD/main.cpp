#include "CacheImage.h"
#include "CacheFont.h"
#include "CacheMusic.h"
#include "Constants/Strings.h"
#include "Constants/UiConstants.h"
#include "WindowEventWrapper.h"
#include "PointerType.h"

#include "States/GameState.h"
#include "States/SplashState.h"
#include "States/MainMenuState.h"

#include "UI/MessageBox.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/Configuration.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/StateManager.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Music.h>
#include <NAS2D/Resource/ResourceCache.h>
#include <NAS2D/Mixer/MixerSDL.h>
#include <NAS2D/Mixer/MixerNull.h>
#include <NAS2D/Renderer/RendererOpenGL.h>

#include <SDL2/SDL.h>

#include <string>
#include <iostream>
#include <fstream>


namespace {
	constexpr auto MinimumWindowSize{NAS2D::Vector{1000, 700}};


	void dumpGraphicsInfo(NAS2D::RendererOpenGL& renderer)
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
		auto& filesystem = NAS2D::Utility<NAS2D::Filesystem>::init<NAS2D::Filesystem>("OutpostHD", "LairWorks");
		filesystem.mountSoftFail("data");
		filesystem.mount(filesystem.findInParents("data", filesystem.basePath()));
		filesystem.mountReadWrite(filesystem.prefPath());

		filesystem.makeDirectory(constants::SaveGamePath);

		NAS2D::Configuration& cf = NAS2D::Utility<NAS2D::Configuration>::init(
			std::map<std::string, NAS2D::Dictionary>{
				{
					"graphics",
					{{
						{"screenwidth", MinimumWindowSize.x},
						{"screenheight", MinimumWindowSize.y},
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
		if (graphics.get<int>("screenwidth") < MinimumWindowSize.x) { graphics.set("screenwidth", MinimumWindowSize.x); }
		if (graphics.get<int>("screenheight") < MinimumWindowSize.y) { graphics.set("screenheight", MinimumWindowSize.y); }
		// Force windowed mode
		graphics.set("fullscreen", false);

		try
		{
			NAS2D::Utility<NAS2D::Mixer>::init<NAS2D::MixerSDL>();
		}
		catch (...)
		{
			NAS2D::Utility<NAS2D::Mixer>::init<NAS2D::MixerNull>();
		}

		WindowEventWrapper windowEventWrapper;

		std::cout << "Starting OpenGL Renderer:" << std::endl;
		auto& renderer = NAS2D::Utility<NAS2D::Renderer>::init<NAS2D::RendererOpenGL>("OutpostHD");

		dumpGraphicsInfo(renderer);
		std::cout << std::endl << "** GAME START **" << std::endl << std::endl;

		renderer.minimumSize(MinimumWindowSize);
		renderer.resizeable(true);

		addCursor(PointerType::Normal, "ui/pointers/normal.png", {0, 0});
		addCursor(PointerType::PlaceTile, "ui/pointers/place_tile.png", {16, 16});
		setCursor(PointerType::Normal);

		Control::setDefaultFont(getFont());
		Control::setDefaultFontBold(getFontBold());
		Control::setImageCache(getImageCache());

		const auto& options = cf["options"];
		if (options.get<bool>("maximized"))
		{
			renderer.maximize();
		}

		trackMars = std::make_unique<NAS2D::Music>("music/mars.ogg");
		NAS2D::Utility<NAS2D::Mixer>::get().playMusic(*trackMars);

		NAS2D::StateManager stateManager;
		stateManager.forceStopAudio(false);

		if (argc > 1)
		{
			std::string filename = constants::SaveGamePath + argv[1] + ".xml";
			if (!filesystem.exists(filename))
			{
				std::cout << "Savegame specified on command line: " << argv[1] << " could not be found." << std::endl;
				stateManager.setState(new MainMenuState());
			}

			NAS2D::Utility<NAS2D::Mixer>::get().stopMusic();

			stateManager.setState(new GameState(filename));
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

	getImageCache().clear();
	NAS2D::Utility<NAS2D::Renderer>::clear();
	std::cout << "OpenGL Renderer Terminated." << std::endl;
	NAS2D::Utility<NAS2D::EventHandler>::clear();
	std::cout << "EventHandler Terminated." << std::endl;
	NAS2D::Utility<NAS2D::Mixer>::clear();
	NAS2D::Utility<NAS2D::Configuration>::clear();
	NAS2D::Utility<NAS2D::Filesystem>::clear();

	#ifdef NDEBUG
	// Reset to stdout again (prevents crashes on exit)
	std::cout.rdbuf(originalCoutBuffer);
	#endif

	SDL_Quit();

	return 0;
}
