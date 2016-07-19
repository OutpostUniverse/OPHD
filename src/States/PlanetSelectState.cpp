#include "PlanetSelectState.h"

#include "GameState.h"

PlanetType PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;

PlanetSelectState::PlanetSelectState():	mFont("fonts/mig6800_8x16.png", 8, 16, 0),
										mTinyFont("fonts/ui-normal.png", 7, 9, -1),
										mMousePointer("ui/pointers/normal.png"),
										mSelect("sfx/click.ogg"),
										mHover("sfx/menu4.ogg"),
										mAiGender(AiVoiceNotifier::AiGender::MALE)
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
	mPlanets[0]->mouseEnter().Connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[0]->mouseExit().Connect(this, &PlanetSelectState::onMousePlanetExit);

	mPlanets[1]->position((int)r.width() / 2 - 64, (int)r.height() / 2 - 64);
	mPlanets[1]->mouseEnter().Connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[1]->mouseExit().Connect(this, &PlanetSelectState::onMousePlanetExit);

	mPlanets[2]->position((((int)r.width() / 4) * 3) - 64, (int)r.height() / 2 - 64);
	mPlanets[2]->mouseEnter().Connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[2]->mouseExit().Connect(this, &PlanetSelectState::onMousePlanetExit);

	PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;

	mMale.type(Button::BUTTON_TOGGLE);
	mMale.font(mTinyFont);
	mMale.text("Male");
	mMale.size(50, 20);
	mMale.position(5, 30);
	mMale.toggle(true);
	mMale.click().Connect(this, &PlanetSelectState::btnMaleClicked);

	mFemale.type(Button::BUTTON_TOGGLE);
	mFemale.font(mTinyFont);
	mFemale.text("Female");
	mFemale.size(50, 20);
	mFemale.position(60, 30);
	mFemale.click().Connect(this, &PlanetSelectState::btnFemaleClicked);

	mPlanetDescription.text("");
	mPlanetDescription.font(mFont);
	mPlanetDescription.size(500, 150);
	mPlanetDescription.position(r.screenCenterX() - 250, r.height() - 175);

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


	r.drawText(mFont, "AI Gender", 5, 5, 255, 255, 255);
	mMale.update();
	mFemale.update();

	mPlanetDescription.update();

	r.drawImage(mMousePointer, mMousePosition.x(), mMousePosition.y());

	if (r.isFading())
		return this;
	else if (PLANET_TYPE_SELECTION != PLANET_TYPE_NONE)
	{
		if (PLANET_TYPE_SELECTION == PLANET_TYPE_MERCURY)
			return new GameState("maps/merc_01", "tsets/mercury.png", mAiGender);
		if (PLANET_TYPE_SELECTION == PLANET_TYPE_MARS)
			return new GameState("maps/mars_04", "tsets/mars.png", mAiGender);
		if (PLANET_TYPE_SELECTION == PLANET_TYPE_GANYMEDE)
			return new GameState("maps/ganymede_01", "tsets/ganymede.png", mAiGender);
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
			Utility<Mixer>::get().playSound(mSelect);
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


void PlanetSelectState::onMousePlanetEnter()
{
	Utility<Mixer>::get().playSound(mHover);

	for (size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i]->mouseHovering())
		{
			if (mPlanets[i]->type() == PLANET_TYPE_GANYMEDE)
				mPlanetDescription.text(constants::PLANET_DESCRIPTION_GANYMEDE);
			if (mPlanets[i]->type() == PLANET_TYPE_MARS)
				mPlanetDescription.text(constants::PLANET_DESCRIPTION_MARS);
			if (mPlanets[i]->type() == PLANET_TYPE_MERCURY)
				mPlanetDescription.text(constants::PLANET_DESCRIPTION_MERCURY);
		}
	}
}



void PlanetSelectState::onMousePlanetExit()
{
	mPlanetDescription.text("");
}


void PlanetSelectState::btnMaleClicked()
{
	Utility<Mixer>::get().playSound(mSelect);
	if (mMale.toggled())
	{
		mFemale.toggle(false);
		mAiGender = AiVoiceNotifier::AiGender::MALE;
	}
	else
	{
		mFemale.toggle(true);
		mAiGender = AiVoiceNotifier::AiGender::FEMALE;
	}
}


void PlanetSelectState::btnFemaleClicked()
{
	Utility<Mixer>::get().playSound(mSelect);
	if (mFemale.toggled())
	{
		mMale.toggle(false);
		mAiGender = AiVoiceNotifier::AiGender::FEMALE;
	}
	else
	{
		mMale.toggle(true);
		mAiGender = AiVoiceNotifier::AiGender::MALE;
	}
}