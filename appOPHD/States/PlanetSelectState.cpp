#include "PlanetSelectState.h"

#include "GameState.h"
#include "MainMenuState.h"

#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../CacheFont.h"
#include "../UI/PlanetImage.h"

#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/PlanetAttributes.h>
#include <libOPHD/XmlSerializer.h>

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Mixer/Mixer.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Math/Angle.h>
#include <NAS2D/Math/Point.h>

#include <cstddef>
#include <limits>
#include <ranges>


namespace
{
	constexpr std::size_t NoSelection{std::numeric_limits<std::size_t>::max()};


	auto attributesToPlanetImages(const std::vector<PlanetAttributes>& attributes)
	{
		const auto& imagePaths = std::ranges::views::transform(attributes, &PlanetAttributes::imagePath);
		return std::vector<PlanetImage>{imagePaths.begin(), imagePaths.end()};
	}
}


PlanetSelectState::PlanetSelectState() :
	mFontBold{getFontMediumBold()},
	mFontTiny{Control::getDefaultFont()},
	mBackground{"sys/bg1.png"},
	mCloud1{"sys/cloud_1.png"},
	mCloud2{"sys/cloud_2.png"},
	mBackgroundMusic{"music/menu.ogg"},
	mSelect{"sfx/click.ogg"},
	mHover{"sfx/menu4.ogg"},
	mReturnState{this},
	mQuit{"Main Menu", {100, 20}, {this, &PlanetSelectState::onQuit}},
	mPlanetDescription{getFontMedium()},
	mPlanetSelection{NoSelection},
	mPlanetAttributes{parsePlanetAttributes("planets/PlanetAttributes.xml")},
	mPlanetImages{attributesToPlanetImages(mPlanetAttributes)}
{
	for (auto& planetImage : mPlanetImages)
	{
		planetImage.mouseEnterHandler({this, &PlanetSelectState::onMousePlanetEnter});
		planetImage.mouseExitHandler({this, &PlanetSelectState::onMousePlanetExit});
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

	for (std::size_t i = 0; i < mPlanetImages.size(); ++i)
	{
		auto& planetImage = mPlanetImages[i];
		planetImage.update();
		const auto& planetName = mPlanetAttributes[i].name;
		renderer.drawText(mFontBold, planetName, planetImage.position() + NAS2D::Vector{64 - (mFontBold.width(planetName) / 2), -mFontBold.height() - 10}, NAS2D::Color::White);
	}

	mQuit.update();

	mPlanetDescription.update();

	renderer.drawText(mFontTiny, constants::Version, NAS2D::Point{-5, -5} + size - mFontTiny.size(constants::Version), NAS2D::Color::White);

	mFade.update();
	mFade.draw(renderer);

	if (mFade.isFading())
	{
		return this;
	}
	else if (mPlanetSelection != NoSelection)
	{
		return new GameState(mPlanetAttributes[mPlanetSelection], Difficulty::Medium);
	}

	return mReturnState;
}


void PlanetSelectState::onMouseDown(NAS2D::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
	for (std::size_t i = 0; i < mPlanetImages.size(); ++i)
	{
		if (mPlanetImages[i].isMouseOver())
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

	for (std::size_t i = 0; i < mPlanetImages.size(); ++i)
	{
		auto& planetImage = mPlanetImages[i];
		if (planetImage.isMouseOver())
		{
			mPlanetDescription.text(mPlanetAttributes[i].description);
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
	for (auto& planetImage : mPlanetImages)
	{
		planetImage.position(planetPosition);
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
