#include "Rectangle.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


Rectangle::Rectangle(NAS2D::Color color, NAS2D::Vector<int> newSize) :
	mColor{color}
{
	size(newSize);
}


void Rectangle::draw() const
{
	if (!visible()) { return; }
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.drawBoxFilled(mRect, mColor);
}
