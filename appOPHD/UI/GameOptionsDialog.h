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
	void onSave();
	void onLoad();
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
	ClickSignal mSignalExit;
	ClickSignal mSignalContinue;
};
