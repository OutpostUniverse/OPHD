#pragma once

#include "NAS2D/NAS2D.h"


#include "../Constants.h"


using namespace NAS2D;


class SplashState : public State
{
public:
	SplashState();
	virtual ~SplashState();

protected:

	void initialize();
	State* update();

private:
	void onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat);
	void onMouseDown(EventHandler::MouseButton button, int x, int y);

	void skipSplash();

private:
	Image				mLogoLairworks;
	Image				mLogoNas2d;
	Image				mLogoOutpostHd;
	Image				mFlare;
	Image				mByline;

	Point_2d			mMousePosition;

	Timer				mTimer;

	State*				mReturnState = this;
};
