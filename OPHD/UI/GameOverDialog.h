#pragma once

#include "UI.h"

class GameOverDialog : public Window
{
public:
	typedef NAS2D::Signals::Signal<> ClickCallback;

public:
	GameOverDialog();
	~GameOverDialog() override;

	ClickCallback& returnToMainMenu() { return mCallback; }

	void update() override;

protected:
	virtual void init();

private:
	GameOverDialog(const GameOverDialog&) = delete;
	GameOverDialog& operator=(const GameOverDialog&) = delete;

private:
	void btnCloseClicked();

private:
	NAS2D::Image	mHeader;

	Button			btnClose;

	ClickCallback	mCallback;
};
