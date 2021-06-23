#include "PlanetSelectState.h"

#include "GameState.h"
#include "MapViewState.h"
#include "MainMenuState.h"

#include "../Constants.h"
#include "../Cache.h"
#include "../XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>

#include <cstddef>
#include <limits>


using namespace NAS2D;


PlanetSelectState::PlanetSelectState() :
	mFontBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium) },
	mTinyFont{ fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal) },
	mBg{"sys/bg1.png"},
	mCloud1{"sys/cloud_1.png"},
	mCloud2{"sys/cloud_2.png"},
	mBgMusic{"music/menu.ogg"},
	mSelect{"sfx/click.ogg"},
	mHover{"sfx/menu4.ogg"},
	mQuit{"Main Menu", {this, &PlanetSelectState::onQuit}},
	mReturnState{this},
	PlanetAttributes(parsePlanetAttributes())
{}


PlanetSelectState::~PlanetSelectState()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect(this, &PlanetSelectState::onMouseDown);
	eventHandler.mouseMotion().disconnect(this, &PlanetSelectState::onMouseMove);
	eventHandler.windowResized().disconnect(this, &PlanetSelectState::onWindowResized);

	for (auto planet : mPlanets) { delete planet; }

	Utility<Mixer>::get().stopAllAudio();
}


void PlanetSelectState::initialize()
{
	auto& eventHandler = Utility<EventHandler>::get();
	eventHandler.mouseButtonDown().connect(this, &PlanetSelectState::onMouseDown);
	eventHandler.mouseMotion().connect(this, &PlanetSelectState::onMouseMove);
	eventHandler.windowResized().connect(this, &PlanetSelectState::onWindowResized);

	for (const auto& planetAttribute : PlanetAttributes)
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

	mQuit.size({100, 20});
	mQuit.position({renderer.size().x - 105, 30});

	mPlanetDescription.text("");
	mPlanetDescription.font(constants::FONT_PRIMARY, constants::FontPrimaryMedium);
	mPlanetDescription.size({550, 200});
	mPlanetDescription.position(NAS2D::Point{viewportSize.x / 2 - 275, viewportSize.y - 225});

	renderer.showSystemPointer(true);
	renderer.fadeIn(constants::FadeSpeed);

	Utility<Mixer>::get().playMusic(mBgMusic);
}


State* PlanetSelectState::update()
{
	auto& renderer = Utility<Renderer>::get();

	const auto size = renderer.size();
	renderer.drawImageStretched(mBg, NAS2D::Rectangle<int>::Create({0, 0}, size));

	float rotation = mTimer.tick() / 1200.0f;
	renderer.drawImageRotated(mCloud1, {-256, -256}, rotation, NAS2D::Color{100, 255, 0, 135});
	renderer.drawImageRotated(mCloud1, NAS2D::Point{size.x - 800, -256}, -rotation, NAS2D::Color{180, 0, 255, 150});

	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		mPlanets[i]->update();
	}

	renderer.drawText(mFontBold, PlanetAttributes[0].name, mPlanets[0]->position() + NAS2D::Vector{64 - (mFontBold.width(PlanetAttributes[0].name) / 2), -mFontBold.height() - 10}, NAS2D::Color::White);
	renderer.drawText(mFontBold, PlanetAttributes[1].name, mPlanets[1]->position() + NAS2D::Vector{64 - (mFontBold.width(PlanetAttributes[1].name) / 2), -mFontBold.height() - 10}, NAS2D::Color::White);
	renderer.drawText(mFontBold, PlanetAttributes[2].name, mPlanets[2]->position() + NAS2D::Vector{64 - (mFontBold.width(PlanetAttributes[2].name) / 2), -mFontBold.height() - 10}, NAS2D::Color::White);

	mQuit.update();

	mPlanetDescription.update();

	renderer.drawText(mTinyFont, constants::Version, NAS2D::Point{-5, -5} + size - mTinyFont.size(constants::Version), NAS2D::Color::White);

	if (renderer.isFading())
	{
		return this;
	}
	else if (mPlanetSelection != constants::NoSelection)
	{
		GameState* gameState = new GameState();
		MapViewState* mapview = new MapViewState(gameState->getMainReportsState(), PlanetAttributes[mPlanetSelection], Difficulty::Medium);
		mapview->setPopulationLevel(MapViewState::PopulationLevel::Large);
		mapview->_initialize();
		mapview->activate();

		gameState->mapviewstate(mapview);

		return gameState;
	}

	return mReturnState;
}


void PlanetSelectState::onMouseDown(EventHandler::MouseButton, int, int)
{
	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i]->mouseHovering())
		{
			Utility<Mixer>::get().playSound(mSelect);
			mPlanetSelection = i;
			Utility<Renderer>::get().fadeOut(constants::FadeSpeed);
			Utility<Mixer>::get().fadeOutMusic(constants::FadeSpeed);
			return;
		}
	}
}


void PlanetSelectState::onMouseMove(int x, int y, int, int)
{
	mMousePosition = {x, y};
}


void PlanetSelectState::onMousePlanetEnter()
{
	Utility<Mixer>::get().playSound(mHover);

	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i]->mouseHovering())
		{
			mPlanetDescription.text(mPlanets[i]->attributes().description);
			break;
		}
	}
}


void PlanetSelectState::onMousePlanetExit()
{
	mPlanetDescription.text("");
}


void PlanetSelectState::onWindowResized(NAS2D::Vector<int> newSize)
{
	const auto middlePosition = NAS2D::Point{0, 0} + (newSize - NAS2D::Vector{128, 128}) / 2;
	const auto offset = NAS2D::Vector{newSize.x / 4, 0};
	mPlanets[0]->position(middlePosition - offset);
	mPlanets[1]->position(middlePosition);
	mPlanets[2]->position(middlePosition + offset);

	mQuit.position(NAS2D::Point{newSize.x - 105, 30});
	mPlanetDescription.position(NAS2D::Point{(newSize.x / 2) - 275, newSize.y - 225});
}


void PlanetSelectState::onQuit()
{
	Utility<Renderer>::get().fadeOut(constants::FadeSpeed);
	mReturnState = new MainMenuState();
}
