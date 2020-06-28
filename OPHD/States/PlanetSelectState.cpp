// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "PlanetSelectState.h"

#include "GameState.h"
#include "MapViewState.h"
#include "MainMenuState.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "NAS2D/Mixer/Mixer.h"
#include "NAS2D/Renderer/Renderer.h"

using namespace NAS2D;

Planet::PlanetType PLANET_TYPE_SELECTION = Planet::PlanetType::PLANET_TYPE_NONE;

static Font* FONT = nullptr;
static Font* FONT_BOLD = nullptr;
static Font* FONT_TINY = nullptr;



class Explosion
{
public:
	Explosion(): mSheet("fx/explosion2.png") {}
	~Explosion() = default;

	void update(int x, int y)
	{
		auto& renderer = Utility<Renderer>::get();

		if (mTimer.accumulator() > 7)
		{
			mFrame = (mFrame + 1) % 64;
			mTimer.reset();
		}

		auto width = 128.0f;
		auto height = 128.0f;
		auto posX = (mFrame % 8u) * width;
		auto posY = (mFrame / 8u) * height;
		auto orientationDegrees = 270.0f;
		renderer.drawSubImageRotated(mSheet, Point<int>{x, y}, {posX, posY, width, height}, orientationDegrees);
	}

private:
	NAS2D::Image mSheet;
	NAS2D::Timer mTimer;

	std::size_t mFrame = 0;
};


Explosion* EXPLODE = nullptr;



PlanetSelectState::PlanetSelectState() :
	mBg{"sys/bg1.png"},
	mStarFlare{"sys/flare_1.png"},
	mDetailFlare{"sys/flare_2.png"},
	mDetailFlare2{"sys/flare_3.png"},
	mCloud1{"sys/cloud_1.png"},
	mCloud2{"sys/cloud_2.png"},
	mBgMusic{"music/menu.ogg"},
	mSelect{"sfx/click.ogg"},
	mHover{"sfx/menu4.ogg"},
	mQuit{"Main Menu"},
	mReturnState{this}
{}


PlanetSelectState::~PlanetSelectState()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.mouseButtonDown().disconnect(this, &PlanetSelectState::onMouseDown);
	e.mouseMotion().disconnect(this, &PlanetSelectState::onMouseMove);
	e.windowResized().disconnect(this, &PlanetSelectState::onWindowResized);

	for (auto planet : mPlanets) { delete planet; }

	Utility<Mixer>::get().stopAllAudio();
}


void PlanetSelectState::initialize()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.mouseButtonDown().connect(this, &PlanetSelectState::onMouseDown);
	e.mouseMotion().connect(this, &PlanetSelectState::onMouseMove);
	e.windowResized().connect(this, &PlanetSelectState::onWindowResized);

	mPlanets.push_back(new Planet(Planet::PlanetType::PLANET_TYPE_MERCURY));
	mPlanets.push_back(new Planet(Planet::PlanetType::PLANET_TYPE_MARS));
	mPlanets.push_back(new Planet(Planet::PlanetType::PLANET_TYPE_GANYMEDE));

	auto& renderer = Utility<Renderer>::get();
	const auto viewportSize = renderer.size().to<int>();
	mPlanets[0]->position(viewportSize.x / 4 - 64, viewportSize.y / 2 - 64);
	mPlanets[0]->mouseEnter().connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[0]->mouseExit().connect(this, &PlanetSelectState::onMousePlanetExit);

	mPlanets[1]->position(viewportSize.x / 2 - 64, viewportSize.y / 2 - 64);
	mPlanets[1]->mouseEnter().connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[1]->mouseExit().connect(this, &PlanetSelectState::onMousePlanetExit);

	mPlanets[2]->position(((viewportSize.x / 4) * 3) - 64, viewportSize.y / 2 - 64);
	mPlanets[2]->mouseEnter().connect(this, &PlanetSelectState::onMousePlanetEnter);
	mPlanets[2]->mouseExit().connect(this, &PlanetSelectState::onMousePlanetExit);

	PLANET_TYPE_SELECTION = Planet::PlanetType::PLANET_TYPE_NONE;

	mQuit.size({100, 20});
	mQuit.position(renderer.width() - 105, 30);
	mQuit.click().connect(this, &PlanetSelectState::btnQuitClicked);

	mPlanetDescription.text("");
	mPlanetDescription.font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	mPlanetDescription.size({550, 200});
	mPlanetDescription.position(renderer.center_x() - 275, renderer.height() - 225);

	renderer.showSystemPointer(true);
	renderer.fadeIn(constants::FADE_SPEED);

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM);
	FONT_TINY = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);


	EXPLODE = new Explosion();

	Utility<Mixer>::get().playMusic(mBgMusic);
}


void PlanetSelectState::drawStar(int x, int y)
{
	float rotation = (mTimer.tick() / 125.0f);
	auto& renderer = Utility<Renderer>::get();
	renderer.drawImageRotated(mStarFlare, static_cast<float>(x), static_cast<float>(y), -rotation * 0.75f, 255, 255, 0, 180);
	renderer.drawImageRotated(mDetailFlare2, static_cast<float>(x), static_cast<float>(y), -rotation * 0.25f, 255, 255, 100, 255);
	renderer.drawImageRotated(mDetailFlare, static_cast<float>(x), static_cast<float>(y), rotation, 255, 255, 255, 255);
}


State* PlanetSelectState::update()
{
	auto& renderer = Utility<Renderer>::get();

	const auto size = renderer.size();
	renderer.drawImageStretched(mBg, {0, 0}, size);

	float _rotation = mTimer.tick() / 1200.0f;
	renderer.drawImageRotated(mCloud1, -256, -256, _rotation, 100, 255, 0, 135);
	renderer.drawImageRotated(mCloud1, size.x - 800, -256, -_rotation, 180, 0, 255, 150);

	drawStar(-40, -55);

	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		mPlanets[i]->update();
	}

	//EXPLODE->update(100, 100);

	renderer.drawText(*FONT_BOLD, "Mercury Type", static_cast<float>(mPlanets[0]->x() + 64 - (FONT_BOLD->width("Mercury Type") / 2)), static_cast<float>(mPlanets[0]->y() - FONT_BOLD->height() - 10), 255, 255, 255);
	renderer.drawText(*FONT_BOLD, "Mars Type", static_cast<float>(mPlanets[1]->x() + 64 - (FONT_BOLD->width("Mars Type") / 2)), static_cast<float>(mPlanets[1]->y() - FONT_BOLD->height() - 10), 255, 255, 255);
	renderer.drawText(*FONT_BOLD, "Ganymede Type", static_cast<float>(mPlanets[2]->x() + 64 - (FONT_BOLD->width("Ganymede Type") / 2)), static_cast<float>(mPlanets[2]->y() - FONT_BOLD->height() - 10), 255, 255, 255);

	renderer.drawText(*FONT, "AI Gender", 5, 5, 255, 255, 255);
	mQuit.update();

	mPlanetDescription.update();

	renderer.drawText(*FONT_TINY, constants::VERSION, size.x - FONT_TINY->width(constants::VERSION) - 5, size.y - FONT_TINY->height() - 5, 255, 255, 255);

	if (renderer.isFading())
	{
		return this;
	}
	else if (PLANET_TYPE_SELECTION != Planet::PlanetType::PLANET_TYPE_NONE)
	{
		std::string map, tileset;
		int dig_depth = 0, max_mines = 0;
		constants::PlanetHostility hostility = constants::PlanetHostility::HOSTILITY_NONE;

		switch (PLANET_TYPE_SELECTION)
		{
		case Planet::PlanetType::PLANET_TYPE_MERCURY:
			map = "maps/merc_01";
			tileset = "tsets/mercury.png";
			dig_depth = mPlanets[0]->digDepth();
			max_mines = mPlanets[0]->maxMines();
			hostility = constants::PlanetHostility::HOSTILITY_HIGH;
			break;

		case Planet::PlanetType::PLANET_TYPE_MARS:
			map = "maps/mars_04";
			tileset = "tsets/mars.png";
			dig_depth = mPlanets[1]->digDepth();
			max_mines = mPlanets[1]->maxMines();
			hostility = constants::PlanetHostility::HOSTILITY_LOW;
			break;

		case Planet::PlanetType::PLANET_TYPE_GANYMEDE:
			map = "maps/ganymede_01";
			tileset = "tsets/ganymede.png";
			dig_depth = mPlanets[2]->digDepth();
			max_mines = mPlanets[2]->maxMines();
			hostility = constants::PlanetHostility::HOSTILITY_MEDIUM;
			break;

		default:
			return mReturnState;
			break;
		}

		MapViewState* mapview = new MapViewState(map, tileset, dig_depth, max_mines, hostility);
		mapview->setPopulationLevel(MapViewState::PopulationLevel::POPULATION_LARGE);
		mapview->_initialize();
		mapview->activate();

		GameState* gameState = new GameState();
		gameState->mapviewstate(mapview);

		return gameState;
	}

	return mReturnState;
}


void PlanetSelectState::onMouseDown(EventHandler::MouseButton /*button*/, int /*x*/, int /*y*/)
{
	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i]->mouseHovering())
		{
			Utility<Mixer>::get().playSound(mSelect);
			PLANET_TYPE_SELECTION = mPlanets[i]->type();
			Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
			Utility<Mixer>::get().fadeOutMusic(constants::FADE_SPEED);
			return;
		}
	}
}


void PlanetSelectState::onMouseMove(int x, int y, int /*rX*/, int /*rY*/)
{
	mMousePosition = {x, y};
}


void PlanetSelectState::onMousePlanetEnter()
{
	Utility<Mixer>::get().playSound(mHover);

	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		// FIXME: Ugly, will be difficult to maintain in the future.
		if (mPlanets[i]->mouseHovering())
		{
			if (mPlanets[i]->type() == Planet::PlanetType::PLANET_TYPE_GANYMEDE) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_GANYMEDE); }
			if (mPlanets[i]->type() == Planet::PlanetType::PLANET_TYPE_MARS) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_MARS); }
			if (mPlanets[i]->type() == Planet::PlanetType::PLANET_TYPE_MERCURY) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_MERCURY); }
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

	mQuit.position(w - 105.0f, 30.0f);
	mPlanetDescription.position((w / 2.0f) - 275.0f, h - 225.0f);
}


void PlanetSelectState::btnQuitClicked()
{
	Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
	mReturnState = new MainMenuState();
}
