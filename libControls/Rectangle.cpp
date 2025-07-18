#include "Rectangle.h"

#include <NAS2D/Renderer/Renderer.h>


Rectangle::Rectangle(NAS2D::Color color, NAS2D::Vector<int> newSize) :
	mColor{color}
{
	size(newSize);
}


void Rectangle::draw(NAS2D::Renderer& renderer) const
{
	if (!visible()) { return; }
	renderer.drawBoxFilled(mRect, mColor);
}
