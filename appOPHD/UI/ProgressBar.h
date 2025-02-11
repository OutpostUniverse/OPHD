#pragma once


namespace NAS2D
{
	template <typename BaseType>
	struct Rectangle;
}


void drawProgressBar(int value, int max, NAS2D::Rectangle<int> rect, int padding = 4);
