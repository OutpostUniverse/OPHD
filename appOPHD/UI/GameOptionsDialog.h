#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>


class GameOptionsDialog : public Window
{
public:
	using ClickHandler = NAS2D::Delegate<void()>;

	GameOptionsDialog(
		ClickHandler saveClickHandler,
		ClickHandler loadClickHandler,
		ClickHandler exitClickHandler,
		ClickHandler continueClickHandler
	);

	void update() override;

private:
	void onHelp();

	void onEnableChange() override;

	Button btnSave;
	Button btnLoad;
	Button btnHelp;
	Button btnExit;
	Button btnContinue;
};
