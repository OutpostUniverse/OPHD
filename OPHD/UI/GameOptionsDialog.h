#pragma once

#include "Core/Window.h"
#include "Core/Button.h"


class GameOptionsDialog : public Window
{
public:
	using ClickSignal = NAS2D::Signal<>;

	GameOptionsDialog();
	~GameOptionsDialog() override;

	void update() override;

	ClickSignal::Source& SaveGame() { return mSignalSave; }
	ClickSignal::Source& LoadGame() { return mSignalLoad; }
	ClickSignal::Source& returnToGame() { return mSignalReturn; }
	ClickSignal::Source& returnToMainMenu() { return mSignalClose; }

private:
	void onLoad();
	void onSave();
	void onHelp();
	void onReturn();
	void onClose();

	void onEnableChange() override;

	Button btnSave;
	Button btnLoad;
	Button btnHelp;
	Button btnReturn;
	Button btnClose;

	ClickSignal mSignalSave;
	ClickSignal mSignalLoad;
	ClickSignal mSignalReturn;
	ClickSignal mSignalClose;
};
