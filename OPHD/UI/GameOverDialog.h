#pragma once

#include "Core/Window.h"
#include "Core/Button.h"


class GameOverDialog : public Window
{
public:
	using ClickCallback = NAS2D::Signal<>;

public:
	GameOverDialog();

	ClickCallback::Source& returnToMainMenu() { return mCallback; }

	void update() override;

private:
	void onClose();

	const NAS2D::Image& mHeader;

	Button btnClose;

	ClickCallback mCallback;
};
