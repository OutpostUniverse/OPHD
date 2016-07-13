#pragma once

#include "NAS2D/NAS2D.h"

#include "Planet.h"

#include "../Constants.h"

#include "../UI/UI.h"

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

	typedef std::vector<Planet*> PlanetPtrList;

private:
	void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);

	void onMousePlanetEnter();

private:
	Font				mFont;

	Image				mMousePointer;

	Sound				mSelect;
	Sound				mHover;

	Point_2d			mMousePosition;

	PlanetPtrList		mPlanets;

	State*				mReturnState;
};
