#pragma once

#include "NAS2D/NAS2D.h"

#include "Planet.h"

#include "../Constants.h"

#include "../UI/UI.h"

#include "../AiVoiceNotifier.h"

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
	void onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat);

	void onMouseDown(EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);

	void onMousePlanetEnter();
	void onMousePlanetExit();

	void onWindowResized(int width, int height);

	void btnMaleClicked();
	void btnFemaleClicked();
	void btnQuitClicked();

	void drawStar(int x, int y);

private:
	Font				mFont;
	Font				mFontBold;
	Font				mTinyFont;

	std::string			mMousePointer;

	Image				mBg;

	Image				mStarFlare;
	Image				mDetailFlare;
	Image				mDetailFlare2;

	Image				mCloud1;
	Image				mCloud2;

	Music				mBgMusic;

	Sound				mSelect;
	Sound				mHover;

	Point_2d			mMousePosition;

	PlanetPtrList		mPlanets;

	Button				mMale;
	Button				mFemale;

	Button				mQuit;

	TextArea			mPlanetDescription;

	Timer				mTimer;

	State*				mReturnState;
};
