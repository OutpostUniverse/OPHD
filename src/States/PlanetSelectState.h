#pragma once

#include "NAS2D/NAS2D.h"

#include "Planet.h"

#include "../UI/UI.h"


class PlanetSelectState : public NAS2D::State
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
	void onMouseDown(NAS2D::EventHandler::MouseButton, int, int);
	void onMouseMove(int, int, int, int);

	void onMousePlanetEnter();
	void onMousePlanetExit();

	void onWindowResized(int, int);

	void btnQuitClicked();

	void drawStar(int, int);

private:
	NAS2D::Image	mBg;

	NAS2D::Image	mStarFlare;
	NAS2D::Image	mDetailFlare;
	NAS2D::Image	mDetailFlare2;

	NAS2D::Image	mCloud1;
	NAS2D::Image	mCloud2;

	NAS2D::Music	mBgMusic;

	NAS2D::Sound	mSelect;
	NAS2D::Sound	mHover;

	NAS2D::Point_2d	mMousePosition;

	PlanetPtrList	mPlanets;

	Button			mQuit;

	TextArea		mPlanetDescription;

	NAS2D::Timer	mTimer;

	NAS2D::State*	mReturnState = this;
};
