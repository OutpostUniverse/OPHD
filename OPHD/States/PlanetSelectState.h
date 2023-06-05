#pragma once

#include "Planet.h"

#include "../Constants/UiConstants.h"

#include <libControls/Button.h>
#include <libControls/TextArea.h>

#include <NAS2D/State.h>
#include <NAS2D/Timer.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Resource/Music.h>
#include <NAS2D/Resource/Sound.h>
#include <NAS2D/Renderer/Fade.h>

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
	void onMouseDown(NAS2D::EventHandler::MouseButton, NAS2D::Point<int> position);

	void onMousePlanetEnter();
	void onMousePlanetExit();

	void onWindowResized(NAS2D::Vector<int> newSize);

	void onQuit();

private:
	const NAS2D::Font& mFontBold;
	const NAS2D::Font& mTinyFont;

	const NAS2D::Image mBg;
	const NAS2D::Image mCloud1;
	const NAS2D::Image mCloud2;

	const NAS2D::Music mBgMusic;

	const NAS2D::Sound mSelect;
	const NAS2D::Sound mHover;

	std::vector<Planet*> mPlanets;

	Button mQuit;

	TextArea mPlanetDescription;

	std::size_t mPlanetSelection{constants::NoSelection};

	NAS2D::Timer mTimer;
	NAS2D::Fade mFade;

	NAS2D::State* mReturnState = this;

	std::vector<Planet::Attributes> PlanetAttributes;
};
