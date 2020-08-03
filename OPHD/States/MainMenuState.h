#pragma once

#include "../Constants.h"
#include "../UI/UI.h"
#include "../UI/FileIo.h"

#include <NAS2D/State.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Resources/Image.h>


/**
 * Implements the main menu screen.
 */
class MainMenuState : public NAS2D::State
{
public:
	MainMenuState();
	~MainMenuState() override;

protected:
	void initialize() override;
	State* update() override;

private:
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onWindowResized(int width, int height);
	void onFadeComplete();

	void positionButtons();
	void disableButtons();
	void enableButtons();

	void btnNewGameClicked();
	void btnContinueGameClicked();
	void btnOptionsClicked();
	void btnHelpClicked();
	void btnQuitClicked();

	void wasDifficultyOkClicked();
	void newGameCancelled();

	void fileIoAction(const std::string& filePath, FileIo::FileOperation fileOp);

private:
	const NAS2D::Image mBgImage;

	FileIo mFileIoDialog; /**< File IO window. */

	Button btnNewGame;
	Button btnContinueGame;
	Button btnOptions;
	Button btnHelp;
	Button btnQuit;

	Label lblVersion;
	NAS2D::State* mReturnState = this;
};
