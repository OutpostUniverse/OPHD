#include "PlanetSelectState.h"

#include "GameState.h"
#include "MapViewState.h"
#include "MainMenuState.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../Cache.h"

#include <libOPHD/XmlSerializer.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>

#include <cstddef>
#include <limits>


PlanetSelectState::PlanetSelectState() :
	mFontBold{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium)},
	mTinyFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mBg{"sys/bg1.png"},
	mCloud1{"sys/cloud_1.png"},
	mCloud2{"sys/cloud_2.png"},
	mBgMusic{"music/menu.ogg"},
	mSelect{"sfx/click.ogg"},
	mHover{"sfx/menu4.ogg"},
	mQuit{"Main Menu", {this, &PlanetSelectState::onQuit}},
	mPlanetDescription{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryMedium)},
	mPlanetSelection{constants::NoSelection},
	mReturnState{this},
	PlanetAttributes(parsePlanetAttributes())
{}


PlanetSelectState::~PlanetSelectState()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &PlanetSelectState::onMouseDown});
	eventHandler.windowResized().disconnect({this, &PlanetSelectState::onWindowResized});

	for (auto* planet : mPlanets) { delete planet; }

	NAS2D::Utility<NAS2D::Mixer>::get().stopAllAudio();
}


void PlanetSelectState::initialize()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &PlanetSelectState::onMouseDown});
	eventHandler.windowResized().connect({this, &PlanetSelectState::onWindowResized});

	for (const auto& planetAttribute : PlanetAttributes)
	{
		mPlanets.push_back(new Planet(planetAttribute));
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto viewportSize = renderer.size().to<int>();
	const auto centralPlanetPosition = NAS2D::Point{-64, -64} + viewportSize / 2;
	const auto sidePlanetOffset = NAS2D::Vector{viewportSize.x / 4, 0};
	mPlanets[0]->position(centralPlanetPosition - sidePlanetOffset);
	mPlanets[0]->mouseEnter().connect({this, &PlanetSelectState::onMousePlanetEnter});
	mPlanets[0]->mouseExit().connect({this, &PlanetSelectState::onMousePlanetExit});

	mPlanets[1]->position(centralPlanetPosition);
	mPlanets[1]->mouseEnter().connect({this, &PlanetSelectState::onMousePlanetEnter});
	mPlanets[1]->mouseExit().connect({this, &PlanetSelectState::onMousePlanetExit});

	mPlanets[2]->position(centralPlanetPosition + sidePlanetOffset);
	mPlanets[2]->mouseEnter().connect({this, &PlanetSelectState::onMousePlanetEnter});
	mPlanets[2]->mouseExit().connect({this, &PlanetSelectState::onMousePlanetExit});

	mQuit.size({100, 20});
	mQuit.position({renderer.size().x - 105, 30});

	mPlanetDescription.text("");
	mPlanetDescription.size({550, 200});
	mPlanetDescription.position(NAS2D::Point{viewportSize.x / 2 - 275, viewportSize.y - 225});

	renderer.showSystemPointer(true);
	mFade.fadeIn(constants::FadeSpeed);

	NAS2D::Utility<NAS2D::Mixer>::get().playMusic(mBgMusic);
}


NAS2D::State* PlanetSelectState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto size = renderer.size();
	renderer.drawImageStretched(mBg, NAS2D::Rectangle{{0, 0}, size});

	float rotation = static_cast<float>(mTimer.tick()) / 1200.0f;
	renderer.drawImageRotated(mCloud1, {-256, -256}, rotation, NAS2D::Color{100, 255, 0, 135});
	renderer.drawImageRotated(mCloud1, NAS2D::Point{size.x - 800, -256}, -rotation, NAS2D::Color{180, 0, 255, 150});

	for (auto* planet : mPlanets)
	{
		planet->update();
	}

	for (auto* planet : mPlanets)
	{
		renderer.drawText(mFontBold, planet->attributes().name, planet->position() + NAS2D::Vector{64 - (mFontBold.width(planet->attributes().name) / 2), -mFontBold.height() - 10}, NAS2D::Color::White);
	}

	mQuit.update();

	mPlanetDescription.update();

	renderer.drawText(mTinyFont, constants::Version, NAS2D::Point{-5, -5} + size - mTinyFont.size(constants::Version), NAS2D::Color::White);

	mFade.update();
	mFade.draw(renderer);

	if (mFade.isFading())
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


void PlanetSelectState::onMouseDown(NAS2D::EventHandler::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i]->mouseHovering())
		{
			NAS2D::Utility<NAS2D::Mixer>::get().playSound(mSelect);
			mPlanetSelection = i;
			mFade.fadeOut(constants::FadeSpeed);
			NAS2D::Utility<NAS2D::Mixer>::get().fadeOutMusic(constants::FadeSpeed);
			return;
		}
	}
}


void PlanetSelectState::onMousePlanetEnter()
{
	NAS2D::Utility<NAS2D::Mixer>::get().playSound(mHover);

	for (auto* planet : mPlanets)
	{
		if (planet->mouseHovering())
		{
			mPlanetDescription.text(planet->attributes().description);
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
	mFade.fadeOut(constants::FadeSpeed);
	mReturnState = new MainMenuState();
}
