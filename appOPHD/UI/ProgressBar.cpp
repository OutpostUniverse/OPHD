#include "ProgressBar.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Color.h>


void drawProgressBar(int value, int max, NAS2D::Rectangle<int> rect, int padding)
{
	const auto clippedValue = std::clamp(value, 0, max);
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.drawBox(rect, NAS2D::Color{0, 185, 0});

	if (max > 0)
	{
		auto innerRect = rect.inset(padding);
		innerRect.size.x *= clippedValue / max;
		renderer.drawBoxFilled(innerRect, NAS2D::Color{0, 100, 0});
	}
}
