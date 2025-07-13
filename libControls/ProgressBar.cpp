#include "ProgressBar.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Math/Rectangle.h>

#include <algorithm>


namespace
{
	const auto borderColor = NAS2D::Color{0, 185, 0};
	const auto barColor = NAS2D::Color{0, 100, 0};
}


ProgressBar::ProgressBar(int max, int value, int padding) :
	mPadding{padding},
	mMax{max},
	mValue{value}
{
	size({mMax + padding * 2, padding * 5});
}


void ProgressBar::draw(NAS2D::Renderer& renderer) const
{
	renderer.drawBox(mRect, borderColor);
	if (mMax > 0)
	{
		const auto clippedValue = std::clamp(mValue, 0, mMax);
		auto innerRect = mRect.inset(mPadding);
		innerRect.size.x = innerRect.size.x * clippedValue / mMax;
		renderer.drawBoxFilled(innerRect, barColor);
	}
}
