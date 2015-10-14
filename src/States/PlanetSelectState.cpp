#include "PlanetSelectState.h"

#include "GameState.h"

#include "../UiConstants.h"

#include <cmath>

const unsigned int FRAME_TIME = 115;

unsigned int PLANET_FRAME_TICK = 0;


enum PlanetTypeSelection
{
	PLANET_TYPE_NONE,
	PLANET_TYPE_MERCURY,
	PLANET_TYPE_MARS,
	PLANET_TYPE_GANYMEDE
};



PlanetTypeSelection PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;



PlanetSelectState::PlanetSelectState():	mFont("fonts/Fresca-Regular.ttf", 16),
										mPlanetMerc("planets/mercury.png"),
										mPlanetMars("planets/mars.png"),
										mPlanetGany("planets/ganymede.png"),
										mMousePointer("ui/pointers/normal.png"),
										mMercTick(0),
										mMarsTick(0),
										mGanyTick(0)
{}


PlanetSelectState::~PlanetSelectState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.keyDown().Disconnect(this, &PlanetSelectState::onKeyDown);
	e.mouseButtonDown().Disconnect(this, &PlanetSelectState::onMouseDown);
	e.mouseButtonUp().Disconnect(this, &PlanetSelectState::onMouseUp);
	e.mouseMotion().Disconnect(this, &PlanetSelectState::onMouseMove);
}


void PlanetSelectState::initialize()
{
	mReturnState = this;

	EventHandler& e = Utility<EventHandler>::get();

	e.keyDown().Connect(this, &PlanetSelectState::onKeyDown);

	e.mouseButtonDown().Connect(this, &PlanetSelectState::onMouseDown);
	e.mouseButtonUp().Connect(this, &PlanetSelectState::onMouseUp);
	e.mouseMotion().Connect(this, &PlanetSelectState::onMouseMove);

	Renderer& r = Utility<Renderer>::get();
	mPlanetMercPos(r.width() / 4 - 64, r.height() / 2 - 64);
	mPlanetMarsPos(r.width() / 2 - 64, r.height() / 2 - 64);
	mPlanetGanyPos(((r.width() / 4) * 3) - 64, r.height() / 2 - 64);

	PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;

	Utility<Renderer>::get().fadeIn(175.0f);

	mTimer.reset();
}


State* PlanetSelectState::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(0, 0, r.getScreenResolution().x(), r.getScreenResolution().y(), 0, 0, 0);

	
	unsigned int acc = mTimer.accumulator();
	if (acc >= FRAME_TIME)
	{
		if (acc >= FRAME_TIME * 2)
			mTimer.reset();
		else
			mTimer.adjust_accumulator(FRAME_TIME);
		
		++PLANET_FRAME_TICK;
	}

	if (pow(mMousePosition.x() - mPlanetMercPos.x() - 64, 2) + pow(mMousePosition.y() - mPlanetMercPos.y() - 64, 2) < 4096)
		mMercTick = PLANET_FRAME_TICK;
	else if (pow(mMousePosition.x() - mPlanetMarsPos.x() - 64, 2) + pow(mMousePosition.y() - mPlanetMarsPos.y() - 64, 2) < 4096)
		mMarsTick = PLANET_FRAME_TICK;
	else if (pow(mMousePosition.x() - mPlanetGanyPos.x() - 64, 2) + pow(mMousePosition.y() - mPlanetGanyPos.y() - 64, 2) < 4096)
		mGanyTick = PLANET_FRAME_TICK;

	r.drawSubImage(mPlanetMerc, mPlanetMercPos.x(), mPlanetMercPos.y(), mMercTick % 4 * 128, ((mMercTick % 16) / 4) * 128, 128, 128);
	r.drawSubImage(mPlanetMars, mPlanetMarsPos.x(), mPlanetMarsPos.y(), mMarsTick % 4 * 128, ((mMarsTick % 16) / 4) * 128, 128, 128);
	r.drawSubImage(mPlanetGany, mPlanetGanyPos.x(), mPlanetGanyPos.y(), mGanyTick % 4 * 128, ((mGanyTick % 16) / 4) * 128, 128, 128);

	r.drawText(mFont, "Mercury Type", mPlanetMercPos.x() + 64 - (mFont.width("Mercury Type") / 2), mPlanetMercPos.y() - mFont.height() - 10, 255, 255, 255);
	r.drawText(mFont, "Mars Type", mPlanetMarsPos.x() + 64 - (mFont.width("Mars Type") / 2), mPlanetMarsPos.y() - mFont.height() - 10, 255, 255, 255);
	r.drawText(mFont, "Ganymede Type", mPlanetGanyPos.x() + 64 - (mFont.width("Ganymede Type") / 2), mPlanetGanyPos.y() - mFont.height() - 10, 255, 255, 255);

	r.drawImage(mMousePointer, mMousePosition.x(), mMousePosition.y());

	if (r.isFading())
		return this;
	else if (PLANET_TYPE_SELECTION != PLANET_TYPE_NONE)
	{
		if (PLANET_TYPE_SELECTION == PLANET_TYPE_MERCURY)
			return new GameState("maps/mercury_01", "tsets/mercury.png");
		if (PLANET_TYPE_SELECTION == PLANET_TYPE_MARS)
			return new GameState("maps/mars_04", "tsets/mars.png");
		if (PLANET_TYPE_SELECTION == PLANET_TYPE_GANYMEDE)
			return new GameState("maps/ganymede_01", "tsets/ganymede.png");
	}

	return this;
}


void PlanetSelectState::onKeyDown(KeyCode key, KeyModifier mod, bool repeat)
{

}


void PlanetSelectState::onMouseDown(MouseButton button, int x, int y)
{
	if (pow(mMousePosition.x() - mPlanetMercPos.x() - 64, 2) + pow(mMousePosition.y() - mPlanetMercPos.y() - 64, 2) < 4096)
	{
		PLANET_TYPE_SELECTION = PLANET_TYPE_MERCURY;
		Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
	}
	else if (pow(mMousePosition.x() - mPlanetMarsPos.x() - 64, 2) + pow(mMousePosition.y() - mPlanetMarsPos.y() - 64, 2) < 4096)
	{
		PLANET_TYPE_SELECTION = PLANET_TYPE_MARS;
		Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
	}
	else if (pow(mMousePosition.x() - mPlanetGanyPos.x() - 64, 2) + pow(mMousePosition.y() - mPlanetGanyPos.y() - 64, 2) < 4096)
	{
		PLANET_TYPE_SELECTION = PLANET_TYPE_GANYMEDE;
		Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
	}
}


void PlanetSelectState::onMouseUp(MouseButton button, int x, int y)
{

}


void PlanetSelectState::onMouseMove(int x, int y, int rX, int rY)
{
	mMousePosition(x, y);
}



