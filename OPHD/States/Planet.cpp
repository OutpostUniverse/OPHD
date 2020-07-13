// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Planet.h"

#include "../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include <stdexcept>


namespace {
	constexpr auto PlanetRadius = 64;
	constexpr auto PlanetSize = NAS2D::Vector{PlanetRadius * 2, PlanetRadius * 2};
}


Planet::Planet(const Attributes& attributes) : 
	mMaxMines(attributes.maxMines),
	mMaxDigDepth(attributes.maxDepth),
	mImage(NAS2D::Image(attributes.imagePath)),
	mType(attributes.type)
{
	if (mType == PlanetType::None || mType == PlanetType::Count) {
		throw std::runtime_error("Instantiated Planet class with an invalid planet type.");
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
	const auto spriteFrameOffset = NAS2D::Point{mTick % 8 * PlanetSize.x, ((mTick % 64) / 8) * PlanetSize.y};
	renderer.drawSubImage(mImage, mPosition.to<float>(), spriteFrameOffset.to<float>(), PlanetSize.to<float>());
}
