#pragma once

#include <string>


namespace NAS2D
{
	class Renderer;
	class Font;

	template <typename BaseType> struct Point;
}


std::size_t trendIndex(int changeValue);
std::string formatDiff(int changeValue);

void drawTrendLabel(NAS2D::Renderer& renderer, const NAS2D::Font& font, int changeValue, NAS2D::Point<int> position);
