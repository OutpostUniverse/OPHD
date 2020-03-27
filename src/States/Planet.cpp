// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Planet.h"

#include "NAS2D/Utility.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Renderer/Renderer.h"

#include <array>


namespace {
	constexpr auto PlanetRadius = 64;
	constexpr auto PlanetSize = NAS2D::Vector<int>{PlanetRadius * 2, PlanetRadius * 2};
}


Planet::Planet(PlanetType type) : mType(type)
{
	// Fixme: This should be in a table vs. a giant switch statement.
	switch (mType)
	{
	case PLANET_TYPE_MERCURY:
		mImage = NAS2D::Image(constants::PLANET_TYPE_MERCURY_PATH);
		mMaxMines = 10;
		mMaxDigDepth = 1;
		break;

	case PLANET_TYPE_MARS:
		mImage = NAS2D::Image(constants::PLANET_TYPE_MARS_PATH);
		mMaxMines = 30;
		mMaxDigDepth = 4;
		break;

	case PLANET_TYPE_GANYMEDE:
		mMaxMines = 15;
		mMaxDigDepth = 2;
		mImage = NAS2D::Image(constants::PLANET_TYPE_GANYMEDE_PATH);
		break;

	default:
		throw std::runtime_error("Instantiated Planet class without a valid planet type.");
		break;
	}

	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect(this, &Planet::onMouseMove);
}


Planet::~Planet()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect(this, &Planet::onMouseMove);
}


bool Planet::pointInCircle(NAS2D::Point<int> point)
{
	const auto offset = point - mPosition - PlanetSize / 2;
	constexpr auto radiusSquared = PlanetRadius * PlanetRadius;
	return ((offset.x * offset.x) + (offset.y * offset.y) <= radiusSquared);
}


void Planet::onMouseMove(int x, int y, int /*rX*/, int /*rY*/)
{
	bool inArea = pointInCircle({x, y});
	if (inArea != mMouseInArea)
	{
		mMouseInArea = inArea;
		mMouseInArea ? mMouseEnterCallback() : mMouseExitCallback();
		mTimer.reset();
	}
}


void Planet::update()
{
	if (mMouseInArea && mTimer.accumulator() >= constants::PLANET_ANIMATION_SPEED)
	{
		mTimer.reset(); // don't care about frame skips.
		++mTick;
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto spriteFrameOffset = NAS2D::Point<int>{mTick % 8 * PlanetSize.x, ((mTick % 64) / 8) * PlanetSize.y};
	renderer.drawSubImage(mImage, mPosition.to<float>(), spriteFrameOffset.to<float>(), PlanetSize.to<float>());
}
