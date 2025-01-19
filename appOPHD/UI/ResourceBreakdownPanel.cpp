#include "ResourceBreakdownPanel.h"

#include "TrendLabel.h"

#include "../Cache.h"
#include "../Resources.h"
#include "../Constants/UiConstants.h"

#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


using namespace NAS2D;


ResourceBreakdownPanel::ResourceBreakdownPanel() :
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)},
	mIcons{imageCache.load("ui/icons.png")},
	mSkin{
		imageCache.load("ui/skin/window_top_left.png"),
		imageCache.load("ui/skin/window_top_middle.png"),
		imageCache.load("ui/skin/window_top_right.png"),
		imageCache.load("ui/skin/window_middle_left.png"),
		imageCache.load("ui/skin/window_middle_middle.png"),
		imageCache.load("ui/skin/window_middle_right.png"),
		imageCache.load("ui/skin/window_bottom_left.png"),
		imageCache.load("ui/skin/window_bottom_middle.png"),
		imageCache.load("ui/skin/window_bottom_right.png")
	}
{
	size({270, 80});
}


void ResourceBreakdownPanel::update()
{
	auto& renderer = Utility<Renderer>::get();
	mSkin.draw(renderer, mRect);

	const auto trendIndex = [](int diff) -> std::size_t {
		return
			(diff == 0) ? 1 :
			(diff > 0) ? 2 : 0;
	};
	const std::array trend{
		std::tuple{Color::Red, Point{0, 64}},
		std::tuple{Color::White, Point{16, 64}},
		std::tuple{constants::PrimaryTextColor, Point{8, 64}},
	};

	const std::array resources
	{
		std::tuple{ResourceImageRectsRefined[0], ResourceNamesRefined[0], mPlayerResources->resources[0], mPreviousResources.resources[0]},
		std::tuple{ResourceImageRectsRefined[1], ResourceNamesRefined[1], mPlayerResources->resources[1], mPreviousResources.resources[1]},
		std::tuple{ResourceImageRectsRefined[2], ResourceNamesRefined[2], mPlayerResources->resources[2], mPreviousResources.resources[2]},
		std::tuple{ResourceImageRectsRefined[3], ResourceNamesRefined[3], mPlayerResources->resources[3], mPreviousResources.resources[3]},
	};

	auto position = mRect.position + NAS2D::Vector{5, 5};
	for (const auto& [imageRect, text, value, oldValue] : resources)
	{
		renderer.drawSubImage(mIcons, position, imageRect);
		renderer.drawText(mFont, text, position + NAS2D::Vector{23, 0}, NAS2D::Color::White);
		const auto valueString = std::to_string(value);
		renderer.drawText(mFont, valueString, position + NAS2D::Vector{195 - mFont.width(valueString), 0}, NAS2D::Color::White);
		const auto diff = value - oldValue;
		const auto& [textColor, iconStartPoint] = trend[trendIndex(diff)];
		const auto changeIconImageRect = NAS2D::Rectangle{iconStartPoint, {8, 8}};
		renderer.drawSubImage(mIcons, position + NAS2D::Vector{215, 3}, changeIconImageRect);
		renderer.drawText(mFont, formatDiff(diff), position + NAS2D::Vector{235, 0}, textColor);
		position.y += 18;
	}
}
