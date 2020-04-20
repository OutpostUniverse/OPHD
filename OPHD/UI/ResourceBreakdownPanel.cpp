// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ResourceBreakdownPanel.h"

#include "../Constants.h"
#include "../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"
#include "NAS2D/StringUtils.h"

using namespace NAS2D;

static Font* FONT = nullptr;


enum ResourceTrend
{
	RESOURCE_TREND_NONE,
	RESOURCE_TREND_UP,
	RESOURCE_TREND_DOWN
};


/**
 * Convenience function for setting a resource trend.
 */
static ResourceTrend compareResources(int src, int dst)
{
	if (src > dst) { return ResourceTrend::RESOURCE_TREND_UP; }
	if (src < dst) { return ResourceTrend::RESOURCE_TREND_DOWN; }
	return ResourceTrend::RESOURCE_TREND_NONE;
}


namespace
{
	std::string formatDiff(int diff)
	{
		return ((diff > 0) ? "+" : "") + std::to_string(diff);
	}
}


ResourceBreakdownPanel::ResourceBreakdownPanel() : mIcons("ui/icons.png")
{
	size({270, 80});

	mSkin.push_back(Image("ui/skin/window_top_left.png"));
	mSkin.push_back(Image("ui/skin/window_top_middle.png"));
	mSkin.push_back(Image("ui/skin/window_top_right.png"));
	mSkin.push_back(Image("ui/skin/window_middle_left.png"));
	mSkin.push_back(Image("ui/skin/window_middle_middle.png"));
	mSkin.push_back(Image("ui/skin/window_middle_right.png"));
	mSkin.push_back(Image("ui/skin/window_bottom_left.png"));
	mSkin.push_back(Image("ui/skin/window_bottom_middle.png"));
	mSkin.push_back(Image("ui/skin/window_bottom_right.png"));

	FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
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
	Renderer& renderer = Utility<Renderer>::get();
	renderer.drawImageRect(rect(), mSkin);

	static std::map<ResourceTrend, Point<int>> ICON_SLICE
	{
		{ ResourceTrend::RESOURCE_TREND_NONE,	Point{16, 64} },
		{ ResourceTrend::RESOURCE_TREND_UP,	Point{8, 64} },
		{ ResourceTrend::RESOURCE_TREND_DOWN,	Point{0, 64} }
	};


	static std::map<ResourceTrend, Color> TEXT_COLOR
	{
		{ ResourceTrend::RESOURCE_TREND_NONE,	Color::White },
		{ ResourceTrend::RESOURCE_TREND_UP,	Color{0, 185, 0} },
		{ ResourceTrend::RESOURCE_TREND_DOWN,	Color::Red }
	};

	const auto commonMetalImageRect = NAS2D::Rectangle{64, 16, 16, 16};
	const auto rareMetalImageRect = NAS2D::Rectangle{80, 16, 16, 16};
	const auto commonMineralImageRect = NAS2D::Rectangle{96, 16, 16, 16};
	const auto rareMineralImageRect = NAS2D::Rectangle{112, 16, 16, 16};

	const std::array resources{
		std::tuple{commonMetalImageRect, "Common Metals", mPlayerResources->commonMetals(), mPreviousResources.commonMetals()},
		std::tuple{rareMetalImageRect, "Rare Metals", mPlayerResources->rareMetals(), mPreviousResources.rareMetals()},
		std::tuple{commonMineralImageRect, "Common Minerals", mPlayerResources->commonMinerals(), mPreviousResources.commonMinerals()},
		std::tuple{rareMineralImageRect, "Rare Minerals", mPlayerResources->rareMinerals(), mPreviousResources.rareMinerals()},
	};

	auto position = rect().startPoint() + NAS2D::Vector{5, 5};
	for (const auto& [imageRect, text, value, oldValue] : resources)
	{
		renderer.drawSubImage(mIcons, position, imageRect);
		renderer.drawText(*FONT, text, position + NAS2D::Vector{23, 0}, NAS2D::Color::White);
		const auto valueString = std::to_string(value);
		renderer.drawText(*FONT, valueString, position + NAS2D::Vector{195 - FONT->width(valueString), 0}, NAS2D::Color::White);
		const auto resourceTrend = compareResources(value, oldValue);
		const auto changeIconImageRect = NAS2D::Rectangle<int>::Create(ICON_SLICE[resourceTrend], NAS2D::Vector{8, 8});
		renderer.drawSubImage(mIcons, position + NAS2D::Vector{215, 3}, changeIconImageRect);
		const auto valueChangeColor = TEXT_COLOR[resourceTrend];
		renderer.drawText(*FONT, formatDiff(value - oldValue), position + NAS2D::Vector{235, 0}, valueChangeColor);
		position.y() += 18;
	}
}
