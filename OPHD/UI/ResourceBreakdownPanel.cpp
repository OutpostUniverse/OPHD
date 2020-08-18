// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ResourceBreakdownPanel.h"

#include "../Cache.h"
#include "../Common.h"
#include "../Constants.h"

#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


using namespace NAS2D;


namespace
{
	enum class ResourceTrend
	{
		None,
		Up,
		Down
	};


	/**
	 * Convenience function for setting a resource trend.
	 */
	static ResourceTrend compareResources(int src, int dst)
	{
		return
			(src > dst) ? ResourceTrend::Up :
			(src < dst) ? ResourceTrend::Down : ResourceTrend::None;
	}


	std::string formatDiff(int diff)
	{
		return ((diff > 0) ? "+" : "") + std::to_string(diff);
	}
}


ResourceBreakdownPanel::ResourceBreakdownPanel() :
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL)},
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


/**
 * Called after all resources are modified from structure updates,
 * mining and production. Allows for resource trend information.
 */
void ResourceBreakdownPanel::resourceCheck()
{
}


void ResourceBreakdownPanel::update()
{
	auto& renderer = Utility<Renderer>::get();
	mSkin.draw(renderer, mRect);

	static std::map<ResourceTrend, Point<int>> ICON_SLICE
	{
		{ ResourceTrend::None, Point{16, 64} },
		{ ResourceTrend::Up, Point{8, 64} },
		{ ResourceTrend::Down, Point{0, 64} }
	};


	static std::map<ResourceTrend, Color> TEXT_COLOR
	{
		{ ResourceTrend::None, Color::White },
		{ ResourceTrend::Up, Color{0, 185, 0} },
		{ ResourceTrend::Down, Color::Red }
	};

	const auto commonMetalImageRect = NAS2D::Rectangle{64, 16, 16, 16};
	const auto rareMetalImageRect = NAS2D::Rectangle{80, 16, 16, 16};
	const auto commonMineralImageRect = NAS2D::Rectangle{96, 16, 16, 16};
	const auto rareMineralImageRect = NAS2D::Rectangle{112, 16, 16, 16};

	const std::array resources
	{
		std::tuple{commonMetalImageRect, ResourceNamesRefined[0], mPlayerResources->resources[0], mPreviousResources.resources[0]},
		std::tuple{rareMetalImageRect, ResourceNamesRefined[2], mPlayerResources->resources[2], mPreviousResources.resources[2]},
		std::tuple{commonMineralImageRect, ResourceNamesRefined[1], mPlayerResources->resources[1], mPreviousResources.resources[1]},
		std::tuple{rareMineralImageRect, ResourceNamesRefined[3], mPlayerResources->resources[3], mPreviousResources.resources[3]},
	};

	auto position = mRect.startPoint() + NAS2D::Vector{5, 5};
	for (const auto& [imageRect, text, value, oldValue] : resources)
	{
		renderer.drawSubImage(mIcons, position, imageRect);
		renderer.drawText(mFont, text, position + NAS2D::Vector{23, 0}, NAS2D::Color::White);
		const auto valueString = std::to_string(value);
		renderer.drawText(mFont, valueString, position + NAS2D::Vector{195 - mFont.width(valueString), 0}, NAS2D::Color::White);
		const auto resourceTrend = compareResources(value, oldValue);
		const auto changeIconImageRect = NAS2D::Rectangle<int>::Create(ICON_SLICE[resourceTrend], NAS2D::Vector{8, 8});
		renderer.drawSubImage(mIcons, position + NAS2D::Vector{215, 3}, changeIconImageRect);
		const auto valueChangeColor = TEXT_COLOR[resourceTrend];
		renderer.drawText(mFont, formatDiff(value - oldValue), position + NAS2D::Vector{235, 0}, valueChangeColor);
		position.y += 18;
	}
}
