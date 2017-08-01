#pragma once

#include "UI.h"

class GameOverDialog : public Window
{
public:

	typedef NAS2D::Signals::Signal0<void> ClickCallback;

public:

	GameOverDialog(Font& font);
	virtual ~GameOverDialog();

	virtual void update();

	ClickCallback& returnToMainMenu() { return mCallback; }

protected:

	virtual void init();

private:

	void btnCloseClicked();

	GameOverDialog();
	GameOverDialog(const GameOverDialog&);
	GameOverDialog& operator=(const GameOverDialog&);

	Image			mHeader;

	Button			btnClose;

	ClickCallback	mCallback;
};
