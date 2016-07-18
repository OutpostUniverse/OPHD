#pragma once

#include "NAS2D/NAS2D.h"


#include "../Constants.h"


using namespace NAS2D;


class SplashState : public State
{
public:

	SplashState();
	~SplashState();

protected:

	void initialize();
	State* update();

private:



private:
	void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);

	void skipSplash();

private:
	Image				mMousePointer;

	Image				mLogoLairworks;
	Image				mLogoNas2d;
	Image				mLogoOutpostHd;

	Point_2d			mMousePosition;

	Music				mSplash;

	Timer				mTimer;

	State*				mReturnState;
};
