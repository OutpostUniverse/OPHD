#include "PlanetImage.h"

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
	if (newMouseEnterHandler) { mMouseEnterSignal.connect(newMouseEnterHandler); }
}


void PlanetImage::mouseExitHandler(MouseEventDelegate newMouseExitHandler)
{
	if (newMouseExitHandler) { mMouseExitSignal.connect(newMouseExitHandler); }
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
		mIsMouseOver ? mMouseEnterSignal() : mMouseExitSignal();
		mTimer.reset();
	}
}


void PlanetImage::update()
{
	if (mIsMouseOver && mTimer.elapsedTicks() >= constants::PlanetAnimationSpeed.milliseconds)
	{
		mTimer.reset(); // don't care about frame skips.
		++mFrameIndex;
	}

	draw();
}


void PlanetImage::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto spriteFrameOffset = NAS2D::Point{mFrameIndex % 16 * PlanetSize.x, ((mFrameIndex % 256) / 16) * PlanetSize.y};
	renderer.drawSubImage(mImage, mRect.position, NAS2D::Rectangle{spriteFrameOffset, PlanetSize});
}
