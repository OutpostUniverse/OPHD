#pragma once

#include "UI.h"

class GameOptionsDialog : public Window
{
public:
	typedef NAS2D::Signals::Signal<> ClickCallback;

public:
	GameOptionsDialog();
	~GameOptionsDialog() override;

	void update() override;

	ClickCallback& SaveGame() { return mCallbackSave; }
	ClickCallback& LoadGame() { return mCallbackLoad; }
	ClickCallback& returnToGame() { return mCallbackReturn; }
	ClickCallback& returnToMainMenu() { return mCallbackClose; }
	

protected:
	virtual void init();

private:
	GameOptionsDialog(const GameOptionsDialog&) = delete;
	GameOptionsDialog& operator=(const GameOptionsDialog&) = delete;

private:
	void btnLoadClicked();
	void btnSaveClicked();
	void btnReturnClicked();
	void btnCloseClicked();

	virtual void enabledChanged() final;

private:
	Button			btnSave;
	Button			btnLoad;
	Button			btnReturn;
	Button			btnClose;

	ClickCallback	mCallbackSave;
	ClickCallback	mCallbackLoad;
	ClickCallback	mCallbackReturn;
	ClickCallback	mCallbackClose;
};
