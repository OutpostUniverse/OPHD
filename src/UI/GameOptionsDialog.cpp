// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GameOptionsDialog.h"

#include "../Constants.h"


GameOptionsDialog::GameOptionsDialog()
{
	text(constants::WINDOW_SYSTEM_TITLE);
	init();
}


GameOptionsDialog::~GameOptionsDialog()
{
	btnSave.click().disconnect(this, &GameOptionsDialog::btnSaveClicked);
	btnLoad.click().disconnect(this, &GameOptionsDialog::btnLoadClicked);
	btnReturn.click().disconnect(this, &GameOptionsDialog::btnReturnClicked);
	btnClose.click().disconnect(this, &GameOptionsDialog::btnCloseClicked);
}


void GameOptionsDialog::init()
{
	position(0, 0);
	size(210, 160);

	add(&btnSave, 5, 25);
	btnSave.text("Save current game");
	btnSave.size(200, 25);
	btnSave.click().connect(this, &GameOptionsDialog::btnSaveClicked);

	add(&btnLoad, 5, 53);
	btnLoad.text("Load a saved game");
	btnLoad.size(200, 25);
	btnLoad.click().connect(this, &GameOptionsDialog::btnLoadClicked);

	add(&btnReturn, 5, 91);
	btnReturn.text("Return to current game");
	btnReturn.size(200, 25);
	btnReturn.click().connect(this, &GameOptionsDialog::btnReturnClicked);

	add(&btnClose, 5, 129);
	btnClose.text("Return to Main Menu");
	btnClose.size(200, 25);
	btnClose.click().connect(this, &GameOptionsDialog::btnCloseClicked);

	anchored(true);
}


void GameOptionsDialog::enabledChanged()
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

void GameOptionsDialog::btnSaveClicked()
{
	mCallbackSave();
}

void GameOptionsDialog::btnLoadClicked()
{
	mCallbackLoad();
}

void GameOptionsDialog::btnReturnClicked()
{
	mCallbackReturn();
}

void GameOptionsDialog::btnCloseClicked()
{
	mCallbackClose();
}
