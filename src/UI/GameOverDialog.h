#pragma once

#include "UI.h"

class GameOverDialog : public Window
{
public:
	typedef NAS2D::Signals::Signal0<void> ClickCallback;

public:
	GameOverDialog(Font& font);
	virtual ~GameOverDialog();

	ClickCallback& returnToMainMenu() { return mCallback; }

	virtual void update();

protected:
	virtual void init();

private:
	GameOverDialog() = delete;
	GameOverDialog(const GameOverDialog&) = delete;
	GameOverDialog& operator=(const GameOverDialog&) = delete;

private:
	void btnCloseClicked();

private:
	Image			mHeader;

	Button			btnClose;

	ClickCallback	mCallback;
};
