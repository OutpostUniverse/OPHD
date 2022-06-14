#include "GameOptionsDialog.h"

#include "../Constants/Strings.h"
#include "../ShellOpenPath.h"
#include <array>


GameOptionsDialog::GameOptionsDialog() :
	Window{constants::WindowSystemTitle},
	btnSave{"Save current game", {this, &GameOptionsDialog::onSave}},
	btnLoad{"Load a saved game", {this, &GameOptionsDialog::onLoad}},
	btnHelp{"Help", {this, &GameOptionsDialog::onHelp}},
	btnReturn{"Return to current game", {this, &GameOptionsDialog::onReturn}},
	btnClose{"Return to Main Menu", {this, &GameOptionsDialog::onClose}}
{
	position({0, 0});
	
	const auto buttons = std::array{&btnSave, &btnLoad, &btnHelp, &btnReturn, &btnClose};
	for (auto button : buttons)
	{
		button->size({buttonWidth, buttonHeight});
	}

	int btnSaveHeight = buttonHeight;
	add(btnSave, {buttonHorizontalMargin, btnSaveHeight});
	int btnLoadHeight = btnSaveHeight + buttonHeight + buttonCloseVerticalMargin;
	add(btnLoad, {buttonHorizontalMargin, btnLoadHeight});
	int btnHelpHeight = btnLoadHeight + buttonHeight + buttonCloseVerticalMargin;
	add(btnHelp, {buttonHorizontalMargin, btnHelpHeight});
	int btnReturnHeight = btnHelpHeight + buttonHeight + buttonLargeVerticalMargin;
	add(btnReturn, {buttonHorizontalMargin, btnReturnHeight});
	int btnCloseHeight = btnReturnHeight + buttonHeight + buttonLargeVerticalMargin;
	add(btnClose, {buttonHorizontalMargin, btnCloseHeight});

	int windowHeight = btnCloseHeight + buttonHeight + buttonCloseVerticalMargin;
	size({buttonWidth + 2 * buttonHorizontalMargin, windowHeight});

	anchored(true);
}


GameOptionsDialog::~GameOptionsDialog()
{
	btnSave.click().disconnect(this, &GameOptionsDialog::onSave);
	btnLoad.click().disconnect(this, &GameOptionsDialog::onLoad);
	btnReturn.click().disconnect(this, &GameOptionsDialog::onReturn);
	btnClose.click().disconnect(this, &GameOptionsDialog::onClose);
}


void GameOptionsDialog::onEnableChange()
{
	btnSave.enabled(enabled());
	btnLoad.enabled(enabled());
	btnReturn.enabled(enabled());
	btnClose.enabled(enabled());
}


void GameOptionsDialog::update()
{
	if (!visible()) { return; }

	Window::update();
}

void GameOptionsDialog::onSave()
{
	mSignalSave();
}

void GameOptionsDialog::onLoad()
{
	mSignalLoad();
}

void GameOptionsDialog::onHelp()
{
	shellOpenPath("https://wiki.outpost2.net/doku.php?id=outposthd:how_to_play");
}

void GameOptionsDialog::onReturn()
{
	mSignalReturn();
}

void GameOptionsDialog::onClose()
{
	mSignalClose();
}
