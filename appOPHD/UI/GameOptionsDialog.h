#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>

#include <cstdint>


namespace NAS2D
{
	enum class KeyCode : uint32_t;
	enum class KeyModifier : uint16_t;
}


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
	~GameOptionsDialog() override;

protected:
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);
	void onHelp();

	void onEnableChange() override;

private:
	ClickHandler mContinueClickHandler;
	Button btnSave;
	Button btnLoad;
	Button btnHelp;
	Button btnExit;
	Button btnContinue;
};
