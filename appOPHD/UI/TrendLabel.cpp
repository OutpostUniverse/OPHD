#include "TrendLabel.h"

#include "../Constants/UiConstants.h"

#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


namespace
{
	const std::array trendColor{
		NAS2D::Color::Red,
		NAS2D::Color::White,
		constants::PrimaryTextColor,
	};
}


std::size_t trendIndex(int changeValue)
{
	return (changeValue < 0) ? 0 :
		(changeValue > 0) ? 2 : 1;
}


std::string formatDiff(int changeValue)
{
	return ((changeValue > 0) ? "+" : "") + std::to_string(changeValue);
}


void drawTrendLabel(NAS2D::Renderer& renderer, const NAS2D::Font& font, int changeValue, NAS2D::Point<int> position)
{
	renderer.drawText(font, formatDiff(changeValue), position, trendColor[trendIndex(changeValue)]);
}
