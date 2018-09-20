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
MainMenuState::MainMenuState() : mBgImage("sys/mainmenu.png")
{}


/**
 * C'tor
 */
MainMenuState::~MainMenuState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.windowResized().disconnect(this, &MainMenuState::onWindowResized);
	e.keyDown().disconnect(this, &MainMenuState::onKeyDown);
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

	btnQuit.text(constants::MAIN_MENU_QUIT);
	btnQuit.fontSize(constants::FONT_PRIMARY_MEDIUM);
	btnQuit.size(200, 30);
	btnQuit.click().connect(this, &MainMenuState::btnQuitClicked);

	mFileIoDialog.setMode(FileIo::FILE_LOAD);
	mFileIoDialog.fileOperation().connect(this, &MainMenuState::fileIoAction);
	mFileIoDialog.anchored(false);
	mFileIoDialog.hide();

	positionButtons();

	Utility<Renderer>::get().fadeIn(constants::FADE_SPEED);
	Utility<Renderer>::get().showSystemPointer(true);
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
	btnQuit.position(start_x, start_y + 105);

	mFileIoDialog.position(static_cast<int>(r.center_x() - mFileIoDialog.width() / 2), static_cast<int>(r.center_y() - mFileIoDialog.height() / 2));
}


/**
 * Disables all buttons in the UI.
 */
void MainMenuState::disableButtons()
{
	btnNewGame.enabled(false);
	btnContinueGame.enabled(false);
	btnOptions.enabled(false);
	btnQuit.enabled(false);
}


/**
 * Enables all buttons in the UI.
 */
void MainMenuState::enableButtons()
{
	btnNewGame.enabled(true);
	btnContinueGame.enabled(true);
	//btnOptions.enabled(true);
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

		Utility<WrapperStack>::get().push(new MapViewState(filename));
		Utility<WrapperStack>::get().top()->_initialize();
		Utility<WrapperStack>::get().top()->activate();

		mReturnState = new GameState();
		Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
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
{
	if (key == EventHandler::KEY_F11)
	{
		Utility<Renderer>::get().fullscreen(!Utility<Renderer>::get().fullscreen());
	}
}


/**
 * Window resize event handler.
 */
void MainMenuState::onWindowResized(int width, int height)
{
	positionButtons();
}


/**
 * Click handler for New Game button.
 */
void MainMenuState::btnNewGameClicked()
{
	if (mFileIoDialog.visible()) { return; }

	Utility<Renderer>::get().fadeOut((float)constants::FADE_SPEED);
	mReturnState = new PlanetSelectState();
	disableButtons();
}


/**
 * Click handler for Continue button.
 */
void MainMenuState::btnContinueGameClicked()
{
	if (mFileIoDialog.visible()) { return; }

	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.show();
}


/**
 * Click handler for Options button.
 */
void MainMenuState::btnOptionsClicked()
{
	if (mFileIoDialog.visible()) { return; }
}


/**
 * Click handler for Quit button.
 */
void MainMenuState::btnQuitClicked()
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
	Renderer& r = Utility<Renderer>::get();

	r.clearScreen(0, 0, 0);
	r.drawImage(mBgImage, r.center_x() - mBgImage.width() / 2, r.center_y() - mBgImage.height() / 2);


	if (!mFileIoDialog.visible())
	{
		r.drawBoxFilled(btnNewGame.positionX() - 5, btnNewGame.positionY() - 5, btnNewGame.width() + 10, (btnQuit.positionY() - btnNewGame.positionY()) + btnQuit.height() + 10, 0, 0, 0, 150);
		r.drawBox(btnNewGame.positionX() - 5, btnNewGame.positionY() - 5, btnNewGame.width() + 10, (btnQuit.positionY() - btnNewGame.positionY()) + btnQuit.height() + 10, 0, 185, 0, 255);

		btnNewGame.update();
		btnContinueGame.update();
		btnOptions.update();
		btnQuit.update();
	}

	mFileIoDialog.update();

	Font* tiny_font = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);

	r.drawText(*tiny_font, constants::VERSION, r.width() - tiny_font->width(constants::VERSION) - 5, r.height() - tiny_font->height() - 5, 255, 255, 255);

	if (r.isFading())
	{
		return this;
	}

	return mReturnState;
}
