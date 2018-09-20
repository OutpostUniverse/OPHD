#pragma once

#include "NAS2D/NAS2D.h"

#include "../Constants.h"
#include "../UI/UI.h"

#include "../UI/FileIo.h"


/**
 * Implements the main menu screen.
 */
class MainMenuState : public NAS2D::State
{
public:
	MainMenuState();
	virtual ~MainMenuState();

protected:
	void initialize();
	State* update();

private:
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onWindowResized(int width, int height);

	void positionButtons();
	void disableButtons();
	void enableButtons();

	void btnNewGameClicked();
	void btnContinueGameClicked();
	void btnOptionsClicked();
	void btnQuitClicked();

	void fileIoAction(const std::string& _file, FileIo::FileOperation _op);

private:
	NAS2D::Image		mBgImage;						/**<  */

	FileIo				mFileIoDialog;					/**< File IO window. */

	Button				btnNewGame;						/**<  */
	Button				btnContinueGame;				/**<  */
	Button				btnOptions;						/**<  */
	Button				btnQuit;						/**<  */

	NAS2D::State*		mReturnState = this;			/**<  */
};