#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>


class GameOptionsDialog : public Window
{
public:
	using ClickSignal = NAS2D::Signal<>;
	using ClickHandler = NAS2D::Delegate<void()>;

	GameOptionsDialog(
		ClickHandler saveClickHandler,
		ClickHandler loadClickHandler,
		ClickHandler exitClickHandler,
		ClickHandler continueClickHandler
	);

	void update() override;

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
