#pragma once

#include "NAS2D/NAS2D.h"

#include "../Strings.h"

using namespace NAS2D;


class PlanetSelectState : public State
{
public:

	PlanetSelectState();
	~PlanetSelectState();

protected:

	void initialize();
	State* update();

private:

	void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseUp(MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);

	Font				mFont;

	Image				mMousePointer;

	Image				mPlanetMerc;
	Image				mPlanetMars;
	Image				mPlanetGany;

	Point_2d			mPlanetMercPos;
	Point_2d			mPlanetMarsPos;
	Point_2d			mPlanetGanyPos;
	
	Point_2d			mMousePosition;

	Timer				mTimer;

	int					mMercTick;
	int					mMarsTick;
	int					mGanyTick;

	State*				mReturnState;
};
