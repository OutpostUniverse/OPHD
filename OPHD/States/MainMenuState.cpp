#include "GameState.h"
#include "MapViewState.h"
#include "MainMenuState.h"
#include "PlanetSelectState.h"

#include "../Cache.h"
#include "../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


MainMenuState::MainMenuState() :
	mBgImage("sys/mainmenu.png"),
	btnNewGame{constants::MAIN_MENU_NEW_GAME},
	btnContinueGame{constants::MAIN_MENU_CONTINUE},
	btnOptions{constants::MAIN_MENU_OPTIONS},
	btnHelp{constants::MAIN_MENU_HELP},
	btnQuit{constants::MAIN_MENU_QUIT},
	lblVersion{constants::VERSION},
	mReturnState(this)
{}


MainMenuState::~MainMenuState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.windowResized().disconnect(this, &MainMenuState::onWindowResized);
	e.keyDown().disconnect(this, &MainMenuState::onKeyDown);

	Utility<Mixer>::get().stopAllAudio();
	Utility<Renderer>::get().fadeComplete().disconnect(this, &MainMenuState::onFadeComplete);
}


/**
 * Initialize function, called once when instantiated.
 */
void MainMenuState::initialize()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.windowResized().connect(this, &MainMenuState::onWindowResized);
	e.keyDown().connect(this, &MainMenuState::onKeyDown);

	btnNewGame.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnNewGame.size({200, 30});
	btnNewGame.click().connect(this, &MainMenuState::onNewGame);

	btnContinueGame.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnContinueGame.size({200, 30});
	btnContinueGame.click().connect(this, &MainMenuState::onContinueGame);

	btnOptions.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnOptions.size({200, 30});
	btnOptions.enabled(false);
	btnOptions.click().connect(this, &MainMenuState::onOptions);

	btnHelp.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnHelp.size({200, 30});
	btnHelp.click().connect(this, &MainMenuState::onHelp);

	btnQuit.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnQuit.size({200, 30});
	btnQuit.click().connect(this, &MainMenuState::onQuit);

	mFileIoDialog.setMode(FileIo::FileOperation::Load);
	mFileIoDialog.fileOperation().connect(this, &MainMenuState::onFileIoAction);
	mFileIoDialog.anchored(false);
	mFileIoDialog.hide();

	const Font* tiny_font = &fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	lblVersion.font(tiny_font);
	lblVersion.color(NAS2D::Color::White);

	positionButtons();
	disableButtons();

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.fadeComplete().connect(this, &MainMenuState::onFadeComplete);
	renderer.fadeOut(0);
	renderer.fadeIn(constants::FADE_SPEED);
	renderer.showSystemPointer(true);

	Mixer& mixer = Utility<Mixer>::get();
	if (!mixer.musicPlaying()) { mixer.playMusic(*trackMars); }
}


/**
 * Repositions buttons based on window size.
 */
void MainMenuState::positionButtons()
{
	auto& renderer = Utility<Renderer>::get();
	const auto center = renderer.center().to<int>();

	auto buttonPosition = center - NAS2D::Vector{100, (35 * 4) / 2};

	btnNewGame.position(buttonPosition);
	buttonPosition.y += 35;
	btnContinueGame.position(buttonPosition);
	buttonPosition.y += 35;
	btnOptions.position(buttonPosition);
	buttonPosition.y += 35;
	btnHelp.position(buttonPosition);
	buttonPosition.y += 35;
	btnQuit.position(buttonPosition);

	mFileIoDialog.position(center - mFileIoDialog.size() / 2);

	lblVersion.position(NAS2D::Point{0, 0} + renderer.size() - lblVersion.size());
}


/**
 * Disables all buttons in the UI.
 */
void MainMenuState::disableButtons()
{
	btnNewGame.enabled(false);
	btnContinueGame.enabled(false);
	btnOptions.enabled(false);
	btnHelp.enabled(false);
	btnQuit.enabled(false);
}


/**
 * Enables all buttons in the UI.
 */
void MainMenuState::enableButtons()
{
	btnNewGame.enabled(true);
	btnContinueGame.enabled(true);
	btnOptions.enabled(false);
	btnHelp.enabled(true);
	btnQuit.enabled(true);
}


/**
 * Event handler for file I/O operations via the FileIO Window.
 */
void MainMenuState::onFileIoAction(const std::string& filePath, FileIo::FileOperation fileOp)
{
	if (fileOp == FileIo::FileOperation::Save)
	{
		return;
	}

	if (filePath.empty())
	{
		return;
	}

	std::string filename = constants::SAVE_GAME_PATH + filePath + ".xml";

	try
	{
		checkSavegameVersion(filename);

		GameState* gameState = new GameState();
		MapViewState* mapview = new MapViewState(gameState->getMainReportsState(), filename);
		mapview->_initialize();
		mapview->activate();

		gameState->mapviewstate(mapview);
		mReturnState = gameState;

		Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
		Utility<Mixer>::get().fadeOutMusic(constants::FADE_SPEED);
	}
	catch (const std::exception& e)
	{
		mReturnState = this;
		doNonFatalErrorMessage("Load Failed", e.what());
	}
}


/**
 * Key down event handler.
 */
void MainMenuState::onKeyDown(NAS2D::EventHandler::KeyCode /*key*/, NAS2D::EventHandler::KeyModifier /*mod*/, bool /*repeat*/)
{}


/**
 * Window resize event handler.
 */
void MainMenuState::onWindowResized(NAS2D::Vector<int> /*newSize*/)
{
	positionButtons();
}


/**
 * Event handler for renderer fading.
 */
void MainMenuState::onFadeComplete()
{
	if (Utility<Renderer>::get().isFaded()) { return; }
	enableButtons();
}


/**
 * Click handler for New Game button.
 */
void MainMenuState::onNewGame()
{
	if (mFileIoDialog.visible()) { return; }

	disableButtons();

	mReturnState = new PlanetSelectState();

	Utility<Renderer>::get().fadeOut(static_cast<float>(constants::FADE_SPEED));
	Utility<Mixer>::get().fadeOutMusic(constants::FADE_SPEED);
}


/**
 * Click handler for Continue button.
 */
void MainMenuState::onContinueGame()
{
	if (mFileIoDialog.visible()) { return; }

	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.show();
}


/**
 * Click handler for Options button.
 */
void MainMenuState::onOptions()
{
	if (mFileIoDialog.visible()) { return; }
}


/**
 * Click handler for the Help button.
 */
void MainMenuState::onHelp()
{
	if (mFileIoDialog.visible()) { return; }

#if defined(_WIN32)
	system("start https://wiki.outpost2.net/doku.php?id=outposthd:how_to_play");
#elif defined(__APPLE__)
	system("open https://wiki.outpost2.net/doku.php?id=outposthd:how_to_play");
#elif defined(__linux__)
	system("xdg-open https://wiki.outpost2.net/doku.php?id=outposthd:how_to_play");
#else
	//#error Open a web page support on the current platform not implemented.
	#pragma message( "Open a web page support on the current platform not implemented." )
#endif
}


/**
 * Click handler for Quit button.
 */
void MainMenuState::onQuit()
{
	if (mFileIoDialog.visible()) { return; }

	disableButtons();
	NAS2D::postQuitEvent();
}


/**
 * Update function -- called each frame.
 */
NAS2D::State* MainMenuState::update()
{
	auto& renderer = Utility<Renderer>::get();

	renderer.clearScreen();

	renderer.drawImage(mBgImage, renderer.center() - mBgImage.size() / 2);

	if (!mFileIoDialog.visible())
	{
		const auto padding = NAS2D::Vector{5, 5};
		const auto menuRect = NAS2D::Rectangle<int>::Create(btnNewGame.rect().startPoint() - padding, btnQuit.rect().endPoint() + padding);
		renderer.drawBoxFilled(menuRect, NAS2D::Color{0, 0, 0, 150});
		renderer.drawBox(menuRect, NAS2D::Color{0, 185, 0, 255});

		btnNewGame.update();
		btnContinueGame.update();
		btnOptions.update();
		btnHelp.update();
		btnQuit.update();
	}

	if (mFileIoDialog.visible())
	{
		mFileIoDialog.update();
	}

	lblVersion.update();

	if (renderer.isFading())
	{
		return this;
	}

	return mReturnState;
}
