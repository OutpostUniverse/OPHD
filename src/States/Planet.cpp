// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Planet.h"

#include "NAS2D/Utility.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Renderer/Renderer.h"

#include <array>


namespace {
	constexpr auto PlanetSize = NAS2D::Vector<int>{128, 128};
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


bool Planet::pointInArea(int x, int y)
{
	// Standard point in circle equation. Magic numbers for a circle diameter of 128.
	// Note: assumes all values are always positive.
	return (pow(x - mPosition.x() - 64, 2) + pow(y - mPosition.y() - 64, 2) <= 4096);
}


void Planet::onMouseMove(int x, int y, int /*rX*/, int /*rY*/)
{
	bool inArea = pointInArea(x, y);
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

	// FIXME:	Table approach would be a lot faster for this instead of using multiplications and modulus operations.
	//			In the limited scope that this class is used it's not really worth it to go through a full implementation
	//			as only a few of these objects will ever be on screen ever and as of 11/1/2015 are only ever used once
	//			during planetary selection at the beginning of the game.
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto spriteFrameOffset = NAS2D::Point<int>{mTick % 8 * PlanetSize.x, ((mTick % 64) / 8) * PlanetSize.y};
	renderer.drawSubImage(mImage, mPosition.to<float>(), spriteFrameOffset.to<float>(), PlanetSize.to<float>());
}
