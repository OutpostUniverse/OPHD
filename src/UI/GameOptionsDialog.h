#pragma once

#include "UI.h"

class GameOptionsDialog : public Window
{
public:

	typedef Gallant::Signal0<void> ClickCallbackSave;
	typedef Gallant::Signal0<void> ClickCallbackLoad;
	typedef Gallant::Signal0<void> ClickCallbackReturn;
	typedef Gallant::Signal0<void> ClickCallbackQuit;

public:

	GameOptionsDialog(Font& font);
	virtual ~GameOptionsDialog();

	virtual void update();

	ClickCallbackSave& SaveGame() { return mCallbackSave; }
	ClickCallbackLoad& LoadGame() { return mCallbackLoad; }
	ClickCallbackReturn& returnToGame() { return mCallbackReturn; }
	ClickCallbackQuit& returnToMainMenu() { return mCallbackClose; }
	

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

	ClickCallbackSave	mCallbackSave;
	ClickCallbackLoad	mCallbackLoad;
	ClickCallbackReturn	mCallbackReturn;
	ClickCallbackQuit	mCallbackClose;
};
