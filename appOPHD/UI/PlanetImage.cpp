#include "PlanetImage.h"

#include <libOPHD/XmlSerializer.h>

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Duration.h>


namespace
{
	constexpr auto PlanetRadius = 64;
	constexpr auto PlanetSize = NAS2D::Vector{PlanetRadius * 2, PlanetRadius * 2};

	constexpr NAS2D::Duration PlanetAnimationSpeed{35};
}


PlanetImage::PlanetImage(const std::string& imagePath) :
	mImage(NAS2D::Image(imagePath))
{
	size(PlanetSize);

	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect({this, &PlanetImage::onMouseMove});
}


PlanetImage::~PlanetImage()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect({this, &PlanetImage::onMouseMove});
}


void PlanetImage::mouseEnterHandler(MouseEventDelegate newMouseEnterHandler)
{
	mMouseEnterHandler = newMouseEnterHandler;
}


void PlanetImage::mouseExitHandler(MouseEventDelegate newMouseExitHandler)
{
	mMouseExitHandler = newMouseExitHandler;
}


bool PlanetImage::pointInCircle(NAS2D::Point<int> point) const
{
	const auto offset = point - mRect.position - PlanetSize / 2;
	constexpr auto radiusSquared = PlanetRadius * PlanetRadius;
	return ((offset.x * offset.x) + (offset.y * offset.y) <= radiusSquared);
}


void PlanetImage::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> /*relative*/)
{
	bool inArea = pointInCircle(position);
	if (inArea != mIsMouseOver)
	{
		mIsMouseOver = inArea;
		const auto& mouseEventHandler = mIsMouseOver ? mMouseEnterHandler : mMouseExitHandler;
		if (mouseEventHandler) { mouseEventHandler(); }
		mTimer.reset();
	}
}


void PlanetImage::update()
{
	if (mIsMouseOver && mTimer.elapsedTicks() >= PlanetAnimationSpeed.milliseconds)
	{
		mTimer.reset(); // don't care about frame skips.
		++mFrameIndex;
	}

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);
}


void PlanetImage::draw(NAS2D::Renderer& renderer) const
{
	const auto spriteFrameOffset = NAS2D::Point{mFrameIndex % 16 * PlanetSize.x, ((mFrameIndex % 256) / 16) * PlanetSize.y};
	renderer.drawSubImage(mImage, mRect.position, NAS2D::Rectangle{spriteFrameOffset, PlanetSize});
}
