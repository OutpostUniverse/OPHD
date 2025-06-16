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

protected:
	void onHelp();

	void onEnableChange() override;

private:
	Button btnSave;
	Button btnLoad;
	Button btnHelp;
	Button btnExit;
	Button btnContinue;
};
