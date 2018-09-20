#pragma once

#include "NAS2D/NAS2D.h"

#include "Planet.h"

#include "../UI/UI.h"

#include "../AiVoiceNotifier.h"


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
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);

	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);

	void onMousePlanetEnter();
	void onMousePlanetExit();

	void onWindowResized(int width, int height);

	void btnMaleClicked();
	void btnFemaleClicked();
	void btnQuitClicked();

	void drawStar(int x, int y);

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

	Button			mMale;
	Button			mFemale;

	Button			mQuit;

	TextArea		mPlanetDescription;

	NAS2D::Timer	mTimer;

	NAS2D::State*	mReturnState = this;
};
