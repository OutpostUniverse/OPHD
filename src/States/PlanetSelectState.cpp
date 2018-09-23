// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "PlanetSelectState.h"

#include "GameState.h"
#include "MapViewState.h"
#include "MainMenuState.h"

#include "../Constants.h"
#include "../FontManager.h"

using namespace NAS2D;

PlanetType PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;
static Font* FONT_TINY = nullptr;


PlanetSelectState::PlanetSelectState():	mBg("sys/bg1.png"),
										mStarFlare("sys/flare_1.png"),
										mDetailFlare("sys/flare_2.png"),
										mDetailFlare2("sys/flare_3.png"),
										mCloud1("sys/cloud_1.png"),
										mCloud2("sys/cloud_2.png"),
										mBgMusic("music/menu.ogg"),
										mSelect("sfx/click.ogg"),
										mHover("sfx/menu4.ogg")
{}


PlanetSelectState::~PlanetSelectState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.keyDown().disconnect(this, &PlanetSelectState::onKeyDown);
	e.mouseButtonDown().disconnect(this, &PlanetSelectState::onMouseDown);
	e.mouseMotion().disconnect(this, &PlanetSelectState::onMouseMove);
	e.windowResized().disconnect(this, &PlanetSelectState::onWindowResized);

	for (size_t i = 0; i < mPlanets.size(); ++i)
	{
		delete mPlanets[i];
	}

	Utility<Mixer>::get().stopAllAudio();
}


void PlanetSelectState::initialize()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.keyDown().connect(this, &PlanetSelectState::onKeyDown);
	e.mouseButtonDown().connect(this, &PlanetSelectState::onMouseDown);
	e.mouseMotion().connect(this, &PlanetSelectState::onMouseMove);
	e.windowResized().connect(this, &PlanetSelectState::onWindowResized);

	mPlanets.push_back(new Planet(PLANET_TYPE_MERCURY));
	mPlanets.push_back(new Planet(PLANET_TYPE_MARS));
	mPlanets.push_back(new Planet(PLANET_TYPE_GANYMEDE));

	Renderer& r = Utility<Renderer>::get();
	mPlanets[0]->position((int)r.width() / 4 - 64, (int)r.height() / 2 - 64);
	mPlanets[0]->mouseEnter().connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[0]->mouseExit().connect(this, &PlanetSelectState::onMousePlanetExit);

	mPlanets[1]->position((int)r.width() / 2 - 64, (int)r.height() / 2 - 64);
	mPlanets[1]->mouseEnter().connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[1]->mouseExit().connect(this, &PlanetSelectState::onMousePlanetExit);

	mPlanets[2]->position((((int)r.width() / 4) * 3) - 64, (int)r.height() / 2 - 64);
	mPlanets[2]->mouseEnter().connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[2]->mouseExit().connect(this, &PlanetSelectState::onMousePlanetExit);

	PLANET_TYPE_SELECTION = PLANET_TYPE_NONE;

	mMale.type(Button::BUTTON_TOGGLE);
	mMale.text("Male");
	mMale.size(50, 20);
	mMale.position(5, 30);
	mMale.toggle(true);
	mMale.click().connect(this, &PlanetSelectState::btnMaleClicked);

	mFemale.type(Button::BUTTON_TOGGLE);
	mFemale.text("Female");
	mFemale.size(50, 20);
	mFemale.position(60, 30);
	mFemale.click().connect(this, &PlanetSelectState::btnFemaleClicked);

	mQuit.text("Main Menu");
	mQuit.size(100, 20);
	mQuit.position(r.width() - 105, 30);
	mQuit.click().connect(this, &PlanetSelectState::btnQuitClicked);

	mPlanetDescription.text("");
	mPlanetDescription.font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	mPlanetDescription.size(550, 200);
	mPlanetDescription.position(r.center_x() - 275, r.height() - 225);

	r.showSystemPointer(true);
	r.fadeIn(constants::FADE_SPEED);

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM);
	FONT_TINY = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);

	Utility<Mixer>::get().playMusic(mBgMusic);
}


void PlanetSelectState::drawStar(int x, int y)
{
	float rotation = (mTimer.tick() / 125.0f);
	Renderer& r = Utility<Renderer>::get();
	r.drawImageRotated(mStarFlare, static_cast<float>(x), static_cast<float>(y), -rotation * 0.75f, 255, 255, 0, 180);
	r.drawImageRotated(mDetailFlare2, static_cast<float>(x), static_cast<float>(y), -rotation * 0.25f, 255, 255, 100, 255);
	r.drawImageRotated(mDetailFlare, static_cast<float>(x), static_cast<float>(y), rotation, 255, 255, 255, 255);
}


State* PlanetSelectState::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawImageStretched(mBg, 0, 0, r.width(), r.height());

	float _rotation = mTimer.tick() / 1200.0f;
	r.drawImageRotated(mCloud1, -256, -256, _rotation, 100, 255, 0, 135);
	r.drawImageRotated(mCloud1, r.width() - 800, -256, -_rotation, 180, 0, 255, 150);

	drawStar(-40, -55);

	for (size_t i = 0; i < mPlanets.size(); ++i)
	{
		mPlanets[i]->update();
	}

	r.drawText(*FONT_BOLD, "Mercury Type", static_cast<float>(mPlanets[0]->x() + 64 - (FONT_BOLD->width("Mercury Type") / 2)), static_cast<float>(mPlanets[0]->y() - FONT_BOLD->height() - 10), 255, 255, 255);
	r.drawText(*FONT_BOLD, "Mars Type", static_cast<float>(mPlanets[1]->x() + 64 - (FONT_BOLD->width("Mars Type") / 2)), static_cast<float>(mPlanets[1]->y() - FONT_BOLD->height() - 10), 255, 255, 255);
	r.drawText(*FONT_BOLD, "Ganymede Type", static_cast<float>(mPlanets[2]->x() + 64 - (FONT_BOLD->width("Ganymede Type") / 2)), static_cast<float>(mPlanets[2]->y() - FONT_BOLD->height() - 10), 255, 255, 255);

	r.drawText(*FONT, "AI Gender", 5, 5, 255, 255, 255);
	mMale.update();
	mFemale.update();
	mQuit.update();

	mPlanetDescription.update();

	r.drawText(*FONT_TINY, constants::VERSION, r.width() - FONT_TINY->width(constants::VERSION) - 5, r.height() - FONT_TINY->height() - 5, 255, 255, 255);

	if (r.isFading())
	{
		return this;
	}
	else if (PLANET_TYPE_SELECTION != PLANET_TYPE_NONE)
	{
		std::string map, tileset;
		int dig_depth = 0, max_mines = 0;

		switch (PLANET_TYPE_SELECTION)
		{
		case PLANET_TYPE_MERCURY:
			map = "maps/merc_01";
			tileset = "tsets/mercury.png";
			dig_depth = mPlanets[0]->digDepth();
			max_mines = mPlanets[0]->maxMines();
			break;

		case PLANET_TYPE_MARS:
			map = "maps/mars_04";
			tileset = "tsets/mars.png";
			dig_depth = mPlanets[1]->digDepth();
			max_mines = mPlanets[1]->maxMines();
			break;

		case PLANET_TYPE_GANYMEDE:
			map = "maps/ganymede_01";
			tileset = "tsets/ganymede.png";
			dig_depth = mPlanets[2]->digDepth();
			max_mines = mPlanets[2]->maxMines();
			break;

		default:
			return mReturnState;
			break;
		}

		MapViewState* _state = new MapViewState(map, tileset, dig_depth, max_mines);
		_state->setPopulationLevel(MapViewState::POPULATION_LARGE);

		Utility<WrapperStack>::get().push(_state);
		Utility<WrapperStack>::get().top()->_initialize();
		Utility<WrapperStack>::get().top()->activate();

		return new GameState();
	}

	return mReturnState;
}


void PlanetSelectState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat)
{
	if (key == EventHandler::KEY_F11)
	{
		Utility<Renderer>::get().fullscreen(!Utility<Renderer>::get().fullscreen());
	}
}


void PlanetSelectState::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	for (size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i]->mouseHovering())
		{
			Utility<Mixer>::get().playSound(mSelect);
			PLANET_TYPE_SELECTION = mPlanets[i]->type();
			Utility<Renderer>::get().fadeOut((float)constants::FADE_SPEED);
			Utility<Mixer>::get().fadeOutMusic(constants::FADE_SPEED);
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
		// FIXME: Ugly, will be difficult to maintain in the future.
		if (mPlanets[i]->mouseHovering())
		{
			if (mPlanets[i]->type() == PLANET_TYPE_GANYMEDE) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_GANYMEDE); }
			if (mPlanets[i]->type() == PLANET_TYPE_MARS) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_MARS); }
			if (mPlanets[i]->type() == PLANET_TYPE_MERCURY) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_MERCURY); }
		}
	}
}


void PlanetSelectState::onMousePlanetExit()
{
	mPlanetDescription.text("");
}


void PlanetSelectState::onWindowResized(int w, int h)
{
	mPlanets[0]->position(w / 4 - 64, h / 2 - 64);
	mPlanets[1]->position(w / 2 - 64, h / 2 - 64);
	mPlanets[2]->position(((w / 4) * 3) - 64, h / 2 - 64);

	mQuit.position(w - 55, 30);
	mPlanetDescription.position((w / 2) - 275, h - 225);
}


void PlanetSelectState::btnMaleClicked()
{
	Utility<Mixer>::get().playSound(mSelect);
	if (mMale.toggled())
	{
		mFemale.toggle(false);
		Utility<AiVoiceNotifier>::get().gender(AiVoiceNotifier::AiGender::MALE);
	}
	else
	{
		mFemale.toggle(true);
		Utility<AiVoiceNotifier>::get().gender(AiVoiceNotifier::AiGender::FEMALE);
	}
}


void PlanetSelectState::btnFemaleClicked()
{
	Utility<Mixer>::get().playSound(mSelect);
	if (mFemale.toggled())
	{
		mMale.toggle(false);
		Utility<AiVoiceNotifier>::get().gender(AiVoiceNotifier::AiGender::FEMALE);
	}
	else
	{
		mMale.toggle(true);
		Utility<AiVoiceNotifier>::get().gender(AiVoiceNotifier::AiGender::MALE);
	}
}


void PlanetSelectState::btnQuitClicked()
{
	Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
	mReturnState = new MainMenuState();
}
