#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>


class GameOptionsDialog : public Window
{
public:
	using ClickSignal = NAS2D::Signal<>;

	GameOptionsDialog();

	void update() override;

	ClickSignal::Source& saveGameDialogButtonSignal() { return mSignalSaveDialog; }
	ClickSignal::Source& loadGameDialogButtonSignal() { return mSignalLoadDialog; }
	ClickSignal::Source& exitToMainMenuButtonSignal() { return mSignalExit; }
	ClickSignal::Source& continuePlayingButtonSignal() { return mSignalContinue; }

private:
	const int buttonHeight = 25;
	const int buttonWidth = 200;
	const int buttonHorizontalMargin = 5;
	const int buttonVerticalMargin = 3;

	void onLoad();
	void onSave();
	void onHelp();
	void onExit();
	void onContinue();

	void onEnableChange() override;

	Button btnSave;
	Button btnLoad;
	Button btnHelp;
	Button btnExit;
	Button btnContinue;

	ClickSignal mSignalSaveDialog;
	ClickSignal mSignalLoadDialog;
	ClickSignal mSignalContinue;
	ClickSignal mSignalExit;
};
