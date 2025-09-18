#pragma once

#include <cstddef>
#include <string>
#include <array>


namespace NAS2D
{
	class Renderer;
	class Font;
	class Image;

	template <typename BaseType> struct Point;
	template <typename BaseType> struct Rectangle;
}


std::size_t trendIndex(int changeValue);
std::string formatDiff(int changeValue);

void drawTrendLabel(NAS2D::Renderer& renderer, const NAS2D::Font& font, int changeValue, NAS2D::Point<int> position);
void drawTrendIcon(NAS2D::Renderer& renderer, const NAS2D::Image& icons, const std::array<NAS2D::Rectangle<int>, 3>& iconImageRects, int changeValue, NAS2D::Point<int> position);
