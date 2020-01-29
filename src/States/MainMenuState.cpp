// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameState.h"
#include "MapViewState.h"
#include "MainMenuState.h"
#include "PlanetSelectState.h"
#include "Wrapper.h"

#include "../Constants.h"
#include "../FontManager.h"

using namespace NAS2D;


/**
 * C'tor
 */
MainMenuState::MainMenuState() : mBgImage("sys/mainmenu.png"), mReturnState(this)
{}


/**
 * C'tor
 */
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

	btnNewGame.text(constants::MAIN_MENU_NEW_GAME);
	btnNewGame.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnNewGame.size(200, 30);
	btnNewGame.click().connect(this, &MainMenuState::btnNewGameClicked);

	btnContinueGame.text(constants::MAIN_MENU_CONTINUE);
	btnContinueGame.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnContinueGame.size(200, 30);
	btnContinueGame.click().connect(this, &MainMenuState::btnContinueGameClicked);

	btnOptions.text(constants::MAIN_MENU_OPTIONS);
	btnOptions.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnOptions.size(200, 30);
	btnOptions.enabled(false);
	btnOptions.click().connect(this, &MainMenuState::btnOptionsClicked);

	btnHelp.text(constants::MAIN_MENU_HELP);
	btnHelp.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnHelp.size(200, 30);
	btnHelp.click().connect(this, &MainMenuState::btnHelpClicked);

	btnQuit.text(constants::MAIN_MENU_QUIT);
	btnQuit.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnQuit.size(200, 30);
	btnQuit.click().connect(this, &MainMenuState::btnQuitClicked);

	mFileIoDialog.setMode(FileIo::FILE_LOAD);
	mFileIoDialog.fileOperation().connect(this, &MainMenuState::fileIoAction);
	mFileIoDialog.anchored(false);
	mFileIoDialog.hide();

	dlgOptions.anchored(true);
	dlgOptions.hide();

	Font* tiny_font = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	auto& r = NAS2D::Utility<NAS2D::Renderer>::get();
	lblVersion.font(tiny_font);
	lblVersion.text(constants::VERSION);
	lblVersion.position(r.width() - tiny_font->width(constants::VERSION) - 5, r.height() - tiny_font->height() - 5);
	lblVersion.color(NAS2D::Color::White);

	positionButtons();
	disableButtons();

	Utility<Renderer>::get().fadeComplete().connect(this, &MainMenuState::onFadeComplete);
	Utility<Renderer>::get().fadeIn(constants::FADE_SPEED);
	Utility<Renderer>::get().showSystemPointer(true);

	extern Music* MARS; /// yuck
	Mixer& mixer = Utility<Mixer>::get();
	if (!mixer.musicPlaying()) { mixer.playMusic(*MARS); }
}


/**
 * Repositions buttons based on window size.
 */
void MainMenuState::positionButtons()
{
	Renderer& r = Utility<Renderer>::get();

	int start_x = r.center_x() - 100;
	int start_y = r.center_y() - ((35 * 4) / 2);

	btnNewGame.position(start_x, start_y);
	btnContinueGame.position(start_x, start_y + 35);
	btnOptions.position(start_x, start_y + 70);
	btnHelp.position(start_x, start_y + 105);
	btnQuit.position(start_x, start_y + 140);

	mFileIoDialog.position(static_cast<int>(r.center_x() - mFileIoDialog.width() / 2), static_cast<int>(r.center_y() - mFileIoDialog.height() / 2));
	dlgOptions.position(static_cast<int>(r.center_x() - dlgOptions.width() / 2), static_cast<int>(r.center_y() - dlgOptions.height() / 2));
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
	btnOptions.enabled(true);
	btnHelp.enabled(true);
	btnQuit.enabled(true);
}


/**
 * Event handler for file I/O operations via the FileIO Window.
 */
void MainMenuState::fileIoAction(const std::string& _file, FileIo::FileOperation _op)
{
	if (_op != FileIo::FILE_LOAD) { return; }
	if (_file.empty()) { return; }

	std::string filename = constants::SAVE_GAME_PATH + _file + ".xml";

	if (!Utility<Filesystem>::get().exists(filename))
	{
		doNonFatalErrorMessage("Load Failed", "File '" + filename + "' was not found.");
		return;
	}

	try
	{
		checkSavegameVersion(filename);

		MapViewState* mapview = new MapViewState(filename);
		mapview->_initialize();
		mapview->activate();

		GameState* gameState = new GameState();
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
void MainMenuState::onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat)
{}


/**
 * Window resize event handler.
 */
void MainMenuState::onWindowResized(int width, int height)
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
void MainMenuState::btnNewGameClicked()
{
	if (mFileIoDialog.visible()) { return; }
	if(dlgOptions.visible()) { return; }

	disableButtons();

	mReturnState = new PlanetSelectState();

	Utility<Renderer>::get().fadeOut((float)constants::FADE_SPEED);
	Utility<Mixer>::get().fadeOutMusic(constants::FADE_SPEED);
}


/**
 * Click handler for Continue button.
 */
void MainMenuState::btnContinueGameClicked()
{
	if (mFileIoDialog.visible()) { return; }
	if(dlgOptions.visible()) { return; }

	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.show();
}


/**
 * Click handler for Options button.
 */
void MainMenuState::btnOptionsClicked()
{
	if (mFileIoDialog.visible()) { return; }
	if(dlgOptions.visible()) { return; }

	dlgOptions.show();
}


/**
 * Click handler for the Help button.
 */
void MainMenuState::btnHelpClicked()
{
	if (mFileIoDialog.visible()) { return; }
	if(dlgOptions.visible()) { return; }

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
void MainMenuState::btnQuitClicked()
{
	if (mFileIoDialog.visible()) { return; }
	if(dlgOptions.visible()) { return; }

	disableButtons();
	NAS2D::postQuitEvent();
}


/**
 * Update function -- called each frame.
 */
NAS2D::State* MainMenuState::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.clearScreen(0, 0, 0);
	r.drawImage(mBgImage, r.center_x() - mBgImage.width() / 2, r.center_y() - mBgImage.height() / 2);


	if (!mFileIoDialog.visible() && !dlgOptions.visible())
	{
		r.drawBoxFilled(btnNewGame.positionX() - 5, btnNewGame.positionY() - 5, btnNewGame.width() + 10, (btnQuit.positionY() - btnNewGame.positionY()) + btnQuit.height() + 10, 0, 0, 0, 150);
		r.drawBox(btnNewGame.positionX() - 5, btnNewGame.positionY() - 5, btnNewGame.width() + 10, (btnQuit.positionY() - btnNewGame.positionY()) + btnQuit.height() + 10, 0, 185, 0, 255);

		btnNewGame.update();
		btnContinueGame.update();
		btnOptions.update();
		btnHelp.update();
		btnQuit.update();
	}

	if(dlgOptions.visible())
	{
		dlgOptions.update();
	}
	else if(mFileIoDialog.visible())
	{
		mFileIoDialog.update();
	}

	lblVersion.update();

	if (r.isFading())
	{
		return this;
	}

	return mReturnState;
}
