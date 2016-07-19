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
	void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);

	void onMousePlanetEnter();
	void onMousePlanetExit();

	void btnMaleClicked();
	void btnFemaleClicked();

private:
	Font				mFont;
	Font				mTinyFont;

	Image				mMousePointer;

	Sound				mSelect;
	Sound				mHover;

	Point_2d			mMousePosition;

	PlanetPtrList		mPlanets;

	Button				mMale;
	Button				mFemale;
	
	TextArea			mPlanetDescription;

	AiVoiceNotifier::AiGender	mAiGender;

	State*				mReturnState;
};
