#pragma once

#include "NAS2D/State.h"
#include "NAS2D/Timer.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Resources/Image.h"
#include "NAS2D/Resources/Music.h"
#include "NAS2D/Resources/Sound.h"
#include "NAS2D/Renderer/Point.h"

#include "Planet.h"

#include "../UI/UI.h"

#include <vector>


class PlanetSelectState : public NAS2D::State
{
public:
	PlanetSelectState();
	~PlanetSelectState() override;

protected:
	void initialize() override;
	State* update() override;

private:
	using PlanetPtrList = std::vector<Planet*>;

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

	NAS2D::Point<int>	mMousePosition;

	PlanetPtrList	mPlanets;

	Button			mQuit;

	TextArea		mPlanetDescription;

	NAS2D::Timer	mTimer;

	NAS2D::State*	mReturnState = this;
};
