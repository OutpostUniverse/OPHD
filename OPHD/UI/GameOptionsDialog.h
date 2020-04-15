#pragma once

#include "UI.h"

class GameOptionsDialog : public Window
{
public:
	using ClickCallback = NAS2D::Signals::Signal<>;

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

	void enabledChanged() override;

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
