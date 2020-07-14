// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "PlanetSelectState.h"

#include "GameState.h"
#include "MapViewState.h"
#include "MainMenuState.h"

#include "../Constants.h"
#include "../FontManager.h"

#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>

using namespace NAS2D;

Planet::PlanetType PLANET_TYPE_SELECTION = Planet::PlanetType::None;

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

namespace {
	std::vector<Planet::Attributes> planetAttributes = {
		{ Planet::Attributes{Planet::PlanetType::Mercury, "planets/planet_d.png", Planet::Hostility::High, 1, 10, "maps/merc_01", "tsets/mercury.png"} },
		{ Planet::Attributes{Planet::PlanetType::Mars, "planets/planet_c.png", Planet::Hostility::Low, 4, 30, "maps/mars_04", "tsets/mars.png"} },
		{ Planet::Attributes{Planet::PlanetType::Ganymede, "planets/planet_e.png", Planet::Hostility::Medium, 2, 15, "maps/ganymede_01", "tsets/ganymede.png"} }
	};
}

void PlanetSelectState::initialize()
{
	EventHandler& e = Utility<EventHandler>::get();
	e.mouseButtonDown().connect(this, &PlanetSelectState::onMouseDown);
	e.mouseMotion().connect(this, &PlanetSelectState::onMouseMove);
	e.windowResized().connect(this, &PlanetSelectState::onWindowResized);

	for (const auto& planetAttribute : planetAttributes)
	{
		mPlanets.push_back(new Planet(planetAttribute));
	}

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

	PLANET_TYPE_SELECTION = Planet::PlanetType::None;

	mQuit.size({100, 20});
	mQuit.position({static_cast<int>(renderer.width()) - 105, 30});
	mQuit.click().connect(this, &PlanetSelectState::btnQuitClicked);

	mPlanetDescription.text("");
	mPlanetDescription.font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	mPlanetDescription.size({550, 200});
	mPlanetDescription.position(NAS2D::Point{renderer.center_x() - 275, renderer.height() - 225});

	renderer.showSystemPointer(true);
	renderer.fadeIn(constants::FADE_SPEED);

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM);
	FONT_BOLD = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM);
	FONT_TINY = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);


	EXPLODE = new Explosion();

	Utility<Mixer>::get().playMusic(mBgMusic);
}


void PlanetSelectState::drawStar(NAS2D::Point<int> point)
{
	float rotation = (mTimer.tick() / 125.0f);
	auto& renderer = Utility<Renderer>::get();
	renderer.drawImageRotated(mStarFlare, point, -rotation * 0.75f, NAS2D::Color{255, 255, 0, 180});
	renderer.drawImageRotated(mDetailFlare2, point, -rotation * 0.25f, NAS2D::Color{255, 255, 100});
	renderer.drawImageRotated(mDetailFlare, point, rotation, NAS2D::Color::White);
}


State* PlanetSelectState::update()
{
	auto& renderer = Utility<Renderer>::get();

	const auto size = renderer.size();
	renderer.drawImageStretched(mBg, {0, 0}, size);

	float rotation = mTimer.tick() / 1200.0f;
	renderer.drawImageRotated(mCloud1, {-256, -256}, rotation, NAS2D::Color{100, 255, 0, 135});
	renderer.drawImageRotated(mCloud1, {size.x - 800, -256}, -rotation, NAS2D::Color{180, 0, 255, 150});

	drawStar({-40, -55});

	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		mPlanets[i]->update();
	}

	//EXPLODE->update(100, 100);

	renderer.drawText(*FONT_BOLD, "Mercury Type", mPlanets[0]->position() + NAS2D::Vector{64 - (FONT_BOLD->width("Mercury Type") / 2), -FONT_BOLD->height() - 10}, NAS2D::Color::White);
	renderer.drawText(*FONT_BOLD, "Mars Type", mPlanets[1]->position() + NAS2D::Vector{64 - (FONT_BOLD->width("Mars Type") / 2), -FONT_BOLD->height() - 10}, NAS2D::Color::White);
	renderer.drawText(*FONT_BOLD, "Ganymede Type", mPlanets[2]->position() + NAS2D::Vector{64 - (FONT_BOLD->width("Ganymede Type") / 2), -FONT_BOLD->height() - 10}, NAS2D::Color::White);

	renderer.drawText(*FONT, "AI Gender", {5, 5}, NAS2D::Color::White);
	mQuit.update();

	mPlanetDescription.update();

	renderer.drawText(*FONT_TINY, constants::VERSION, {size.x - FONT_TINY->width(constants::VERSION) - 5, size.y - FONT_TINY->height() - 5}, NAS2D::Color::White);

	if (renderer.isFading())
	{
		return this;
	}
	else if (PLANET_TYPE_SELECTION != Planet::PlanetType::None)
	{
		std::string map, tileset;
		int dig_depth = 0, max_mines = 0;
		Planet::Hostility hostility = Planet::Hostility::None;

		switch (PLANET_TYPE_SELECTION)
		{
		case Planet::PlanetType::Mercury:
			map = mPlanets[0]->mapImagePath();
			tileset = mPlanets[0]->tilesetPath();
			dig_depth = mPlanets[0]->digDepth();
			max_mines = mPlanets[0]->maxMines();
			hostility = mPlanets[0]->hostility();
			break;

		case Planet::PlanetType::Mars:
			map = mPlanets[1]->mapImagePath();
			tileset = mPlanets[1]->tilesetPath();
			dig_depth = mPlanets[1]->digDepth();
			max_mines = mPlanets[1]->maxMines();
			hostility = mPlanets[1]->hostility();
			break;

		case Planet::PlanetType::Ganymede:
			map = mPlanets[2]->mapImagePath();
			tileset = mPlanets[2]->tilesetPath();
			dig_depth = mPlanets[2]->digDepth();
			max_mines = mPlanets[2]->maxMines();
			hostility = mPlanets[2]->hostility();
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
			if (mPlanets[i]->type() == Planet::PlanetType::Ganymede) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_GANYMEDE); }
			if (mPlanets[i]->type() == Planet::PlanetType::Mars) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_MARS); }
			if (mPlanets[i]->type() == Planet::PlanetType::Mercury) { mPlanetDescription.text(constants::PLANET_DESCRIPTION_MERCURY); }
		}
	}
}


void PlanetSelectState::onMousePlanetExit()
{
	mPlanetDescription.text("");
}


void PlanetSelectState::onWindowResized(int w, int h)
{
	const auto middlePosition = NAS2D::Point{0, 0} + (NAS2D::Vector{w, h} - NAS2D::Vector{128, 128}) / 2;
	const auto offset = NAS2D::Vector{w / 4, 0};
	mPlanets[0]->position(middlePosition - offset);
	mPlanets[1]->position(middlePosition);
	mPlanets[2]->position(middlePosition + offset);

	mQuit.position(NAS2D::Point{w - 105, 30});
	mPlanetDescription.position(NAS2D::Point{(w / 2) - 275, h - 225});
}


void PlanetSelectState::btnQuitClicked()
{
	Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
	mReturnState = new MainMenuState();
}
