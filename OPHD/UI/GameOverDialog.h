#pragma once

#include "Core/Window.h"
#include "Core/Button.h"


class GameOverDialog : public Window
{
public:
	using ClickCallback = NAS2D::Signals::Signal<>;

public:
	GameOverDialog();
	GameOverDialog(const GameOverDialog&) = delete;
	GameOverDialog& operator=(const GameOverDialog&) = delete;

	ClickCallback& returnToMainMenu() { return mCallback; }

	void update() override;

private:
	void btnCloseClicked();

private:
	const NAS2D::Image& mHeader;

	Button btnClose;

	ClickCallback mCallback;
};
