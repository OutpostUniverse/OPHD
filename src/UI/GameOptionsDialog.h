#pragma once

#include "UI.h"

class GameOptionsDialog : public Window
{
public:

	typedef NAS2D::Signals::Signal0<void> ClickCallback;

public:

	GameOptionsDialog(Font& font);
	virtual ~GameOptionsDialog();

	virtual void update();

	ClickCallback& SaveGame() { return mCallbackSave; }
	ClickCallback& LoadGame() { return mCallbackLoad; }
	ClickCallback& returnToGame() { return mCallbackReturn; }
	ClickCallback& returnToMainMenu() { return mCallbackClose; }
	

protected:

	virtual void init();

private:

	void btnLoadClicked();
	void btnSaveClicked();
	void btnReturnClicked();
	void btnCloseClicked();

	GameOptionsDialog();
	GameOptionsDialog(const GameOptionsDialog&);
	GameOptionsDialog& operator=(const GameOptionsDialog&);

	Button			btnSave;
	Button			btnLoad;
	Button			btnReturn;
	Button			btnClose;

	ClickCallback	mCallbackSave;
	ClickCallback	mCallbackLoad;
	ClickCallback	mCallbackReturn;
	ClickCallback	mCallbackClose;
};
