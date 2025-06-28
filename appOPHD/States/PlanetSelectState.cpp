#include "PlanetSelectState.h"

#include "GameState.h"
#include "MainMenuState.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../Cache.h"

#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/XmlSerializer.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Angle.h>

#include <cstddef>
#include <limits>


namespace
{
	auto attributesToPlanets(const std::vector<PlanetAttributes>& attributes)
	{
		return std::vector<Planet>{attributes.cbegin(), attributes.cend()};
	}
}


const std::size_t PlanetSelectState::NoSelection{std::numeric_limits<std::size_t>::max()};


PlanetSelectState::PlanetSelectState() :
	mFontBold{fontCache.load(constants::FontPrimaryBold, constants::FontPrimaryMedium)},
	mTinyFont{Control::getDefaultFont()},
	mBackground{"sys/bg1.png"},
	mCloud1{"sys/cloud_1.png"},
	mCloud2{"sys/cloud_2.png"},
	mBackgroundMusic{"music/menu.ogg"},
	mSelect{"sfx/click.ogg"},
	mHover{"sfx/menu4.ogg"},
	mQuit{"Main Menu", {100, 20}, {this, &PlanetSelectState::onQuit}},
	mPlanetDescription{fontCache.load(constants::FontPrimary, constants::FontPrimaryMedium)},
	mPlanetSelection{NoSelection},
	mReturnState{this},
	mPlanets{attributesToPlanets(parsePlanetAttributes("planets/PlanetAttributes.xml"))}
{
	for (auto& planet : mPlanets)
	{
		planet.mouseEnter().connect({this, &PlanetSelectState::onMousePlanetEnter});
		planet.mouseExit().connect({this, &PlanetSelectState::onMousePlanetExit});
	}

	mPlanetDescription.size({550, 200});
}


PlanetSelectState::~PlanetSelectState()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &PlanetSelectState::onMouseDown});
	eventHandler.windowResized().disconnect({this, &PlanetSelectState::onWindowResized});

	NAS2D::Utility<NAS2D::Mixer>::get().stopAllAudio();
}


void PlanetSelectState::initialize()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &PlanetSelectState::onMouseDown});
	eventHandler.windowResized().connect({this, &PlanetSelectState::onWindowResized});

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto viewportSize = renderer.size().to<int>();
	onWindowResized(viewportSize);

	mPlanetDescription.text("");

	renderer.showSystemPointer(true);
	mFade.fadeIn(constants::FadeSpeed);

	NAS2D::Utility<NAS2D::Mixer>::get().playMusic(mBackgroundMusic);
}


NAS2D::State* PlanetSelectState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const auto size = renderer.size();
	renderer.drawImageStretched(mBackground, NAS2D::Rectangle{{0, 0}, size});

	auto rotation = NAS2D::Angle::degrees(static_cast<float>(mTimer.tick()) / 1200.0f);
	renderer.drawImageRotated(mCloud1, {-256, -256}, rotation, NAS2D::Color{100, 255, 0, 135});
	renderer.drawImageRotated(mCloud1, NAS2D::Point{size.x - 800, -256}, -rotation, NAS2D::Color{180, 0, 255, 150});

	for (auto& planet : mPlanets)
	{
		planet.update();
		renderer.drawText(mFontBold, planet.attributes().name, planet.position() + NAS2D::Vector{64 - (mFontBold.width(planet.attributes().name) / 2), -mFontBold.height() - 10}, NAS2D::Color::White);
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
	else if (mPlanetSelection != NoSelection)
	{
		return new GameState(mPlanets[mPlanetSelection].attributes(), Difficulty::Medium);
	}

	return mReturnState;
}


void PlanetSelectState::onMouseDown(NAS2D::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
	for (std::size_t i = 0; i < mPlanets.size(); ++i)
	{
		if (mPlanets[i].mouseHovering())
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

	for (const auto& planet : mPlanets)
	{
		if (planet.mouseHovering())
		{
			mPlanetDescription.text(planet.attributes().description);
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
	const auto offset = NAS2D::Vector{newSize.x / 4, 0};
	auto planetPosition = NAS2D::Point{0, 0} + (newSize - NAS2D::Vector{128, 128}) / 2 - offset;
	for (auto& planet : mPlanets)
	{
		planet.position(planetPosition);
		planetPosition += offset;
	}

	mQuit.position(NAS2D::Point{newSize.x - 105, 30});
	mPlanetDescription.position(NAS2D::Point{(newSize.x / 2) - 275, newSize.y - 225});
}


void PlanetSelectState::onQuit()
{
	mFade.fadeOut(constants::FadeSpeed);
	mReturnState = new MainMenuState();
}
