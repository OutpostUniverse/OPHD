#include "PlanetSelectState.h"

#include "GameState.h"

PlanetType PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;

PlanetSelectState::PlanetSelectState():	mFont("fonts/Fresca-Regular.ttf", 16),
										mMousePointer("ui/pointers/normal.png")
{}


PlanetSelectState::~PlanetSelectState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.keyDown().Disconnect(this, &PlanetSelectState::onKeyDown);
	e.mouseButtonDown().Disconnect(this, &PlanetSelectState::onMouseDown);
	e.mouseMotion().Disconnect(this, &PlanetSelectState::onMouseMove);

	for (size_t i = 0; i < mPlanets.size(); ++i)
		delete mPlanets[i];
}


void PlanetSelectState::initialize()
{
	mReturnState = this;

	EventHandler& e = Utility<EventHandler>::get();

	e.keyDown().Connect(this, &PlanetSelectState::onKeyDown);

	e.mouseButtonDown().Connect(this, &PlanetSelectState::onMouseDown);
	e.mouseMotion().Connect(this, &PlanetSelectState::onMouseMove);

	Renderer& r = Utility<Renderer>::get();
	mPlanets.push_back(new Planet(PLANET_TYPE_MERCURY));
	mPlanets.push_back(new Planet(PLANET_TYPE_MARS));
	mPlanets.push_back(new Planet(PLANET_TYPE_GANYMEDE));

	mPlanets[0]->position((int)r.width() / 4 - 64, (int)r.height() / 2 - 64);
	mPlanets[1]->position((int)r.width() / 2 - 64, (int)r.height() / 2 - 64);
	mPlanets[2]->position((((int)r.width() / 4) * 3) - 64, (int)r.height() / 2 - 64);

	PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;

	Utility<Renderer>::get().fadeIn(175.0f);
}


State* PlanetSelectState::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(0, 0, r.getScreenResolution().x(), r.getScreenResolution().y(), 0, 0, 0);

	for (size_t i = 0; i < mPlanets.size(); ++i)
		mPlanets[i]->update();
	
	r.drawText(mFont, "Mercury Type", mPlanets[0]->x() + 64 - (mFont.width("Mercury Type") / 2), mPlanets[0]->y() - mFont.height() - 10, 255, 255, 255);
	r.drawText(mFont, "Mars Type", mPlanets[1]->x() + 64 - (mFont.width("Mars Type") / 2), mPlanets[1]->y() - mFont.height() - 10, 255, 255, 255);
	r.drawText(mFont, "Ganymede Type", mPlanets[2]->x() + 64 - (mFont.width("Ganymede Type") / 2), mPlanets[2]->y() - mFont.height() - 10, 255, 255, 255);

	r.drawImage(mMousePointer, mMousePosition.x(), mMousePosition.y());

	if (r.isFading())
		return this;
	else if (PLANET_TYPE_SELECTION != PLANET_TYPE_NONE)
	{
		if (PLANET_TYPE_SELECTION == PLANET_TYPE_MERCURY)
			return new GameState("maps/merc_01", "tsets/mercury.png");
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
	
	for (size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i]->mouseHovering())
		{
			PLANET_TYPE_SELECTION = mPlanets[i]->type();
			Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
			return;
		}
	}
}


void PlanetSelectState::onMouseMove(int x, int y, int rX, int rY)
{
	mMousePosition(x, y);
}



