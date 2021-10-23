#include "GameOptionsDialog.h"

#include "../Constants/Strings.h"

#include <array>


GameOptionsDialog::GameOptionsDialog() :
	Window{constants::WindowSystemTitle},
	btnSave{"Save current game", {this, &GameOptionsDialog::onSave}},
	btnLoad{"Load a saved game", {this, &GameOptionsDialog::onLoad}},
	btnReturn{"Return to current game", {this, &GameOptionsDialog::onReturn}},
	btnClose{"Return to Main Menu", {this, &GameOptionsDialog::onClose}}
{
	position({0, 0});
	size({210, 160});

	const auto buttons = std::array{&btnSave, &btnLoad, &btnReturn, &btnClose};
	for (auto button : buttons)
	{
		button->size({200, 25});
	}

	add(btnSave, {5, 25});
	add(btnLoad, {5, 53});
	add(btnReturn, {5, 91});
	add(btnClose, {5, 129});

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

void GameOptionsDialog::onReturn()
{
	mSignalReturn();
}

void GameOptionsDialog::onClose()
{
	mSignalClose();
}
