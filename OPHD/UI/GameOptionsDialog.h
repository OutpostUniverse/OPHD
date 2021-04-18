#pragma once

#include "Core/Window.h"
#include "Core/Button.h"


class GameOptionsDialog : public Window
{
public:
	using ClickCallback = NAS2D::Signal<>;

	GameOptionsDialog();
	~GameOptionsDialog() override;

	void update() override;

	ClickCallback& SaveGame() { return mCallbackSave; }
	ClickCallback& LoadGame() { return mCallbackLoad; }
	ClickCallback& returnToGame() { return mCallbackReturn; }
	ClickCallback& returnToMainMenu() { return mCallbackClose; }

private:
	void btnLoadClicked();
	void btnSaveClicked();
	void btnReturnClicked();
	void btnCloseClicked();

	void enabledChanged() override;

	Button btnSave;
	Button btnLoad;
	Button btnReturn;
	Button btnClose;

	ClickCallback mCallbackSave;
	ClickCallback mCallbackLoad;
	ClickCallback mCallbackReturn;
	ClickCallback mCallbackClose;
};
