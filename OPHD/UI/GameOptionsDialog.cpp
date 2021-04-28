#include "GameOptionsDialog.h"

#include "../Constants.h"


GameOptionsDialog::GameOptionsDialog() :
	Window{constants::WINDOW_SYSTEM_TITLE},
	btnSave{"Save current game"},
	btnLoad{"Load a saved game"},
	btnReturn{"Return to current game"},
	btnClose{"Return to Main Menu"}
{
	position({0, 0});
	size({210, 160});

	add(btnSave, {5, 25});
	btnSave.size({200, 25});
	btnSave.click().connect(this, &GameOptionsDialog::onSave);

	add(btnLoad, {5, 53});
	btnLoad.size({200, 25});
	btnLoad.click().connect(this, &GameOptionsDialog::onLoad);

	add(btnReturn, {5, 91});
	btnReturn.size({200, 25});
	btnReturn.click().connect(this, &GameOptionsDialog::onReturn);

	add(btnClose, {5, 129});
	btnClose.size({200, 25});
	btnClose.click().connect(this, &GameOptionsDialog::onClose);

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
