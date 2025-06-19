#include "Planet.h"

#include "../Constants/UiConstants.h"

#include <libOPHD/XmlSerializer.h>

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Rectangle.h>


namespace
{
	constexpr auto PlanetRadius = 64;
	constexpr auto PlanetSize = NAS2D::Vector{PlanetRadius * 2, PlanetRadius * 2};
}


Planet::Planet(const PlanetAttributes& attributes) :
	mAttributes(attributes),
	mImage(NAS2D::Image(attributes.imagePath))
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect({this, &Planet::onMouseMove});
}


Planet::~Planet()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect({this, &Planet::onMouseMove});
}


bool Planet::pointInCircle(NAS2D::Point<int> point) const
{
	const auto offset = point - mPosition - PlanetSize / 2;
	constexpr auto radiusSquared = PlanetRadius * PlanetRadius;
	return ((offset.x * offset.x) + (offset.y * offset.y) <= radiusSquared);
}


void Planet::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	bool inArea = pointInCircle(position);
	if (inArea != mMouseInArea)
	{
		mMouseInArea = inArea;
		mMouseInArea ? mMouseEnterSignal() : mMouseExitSignal();
		mTimer.reset();
	}
}


void Planet::update()
{
	if (mMouseInArea && mTimer.elapsedTicks() >= constants::PlanetAnimationSpeed.milliseconds)
	{
		mTimer.reset(); // don't care about frame skips.
		++mTick;
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto spriteFrameOffset = NAS2D::Point{mTick % 16 * PlanetSize.x, ((mTick % 256) / 16) * PlanetSize.y};
	renderer.drawSubImage(mImage, mPosition, NAS2D::Rectangle{spriteFrameOffset, PlanetSize});
}
