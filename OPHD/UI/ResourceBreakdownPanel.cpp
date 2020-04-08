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

static ResourceTrend	COMMON_METALS = RESOURCE_TREND_NONE;
static ResourceTrend	COMMON_MINERALS = RESOURCE_TREND_NONE;
static ResourceTrend	RARE_METALS = RESOURCE_TREND_NONE;
static ResourceTrend	RARE_MINERALS = RESOURCE_TREND_NONE;


static Color			COMMON_MET_COL = Color::White;
static Color			COMMON_MIN_COL = Color::White;
static Color			RARE_MET_COL = Color::White;
static Color			RARE_MIN_COL = Color::White;


std::map<ResourceTrend, Point<float>> ICON_SLICE
{
	{ RESOURCE_TREND_NONE,	Point<float>(16.0f, 64.0f) },
	{ RESOURCE_TREND_UP,	Point<float>(8.0f, 64.0f) },
	{ RESOURCE_TREND_DOWN,	Point<float>(0.0f, 64.0f) }
};


std::map<ResourceTrend, Color> TEXT_COLOR
{
	{ RESOURCE_TREND_NONE,	Color::White },
	{ RESOURCE_TREND_UP,	Color{0, 185, 0} },
	{ RESOURCE_TREND_DOWN,	Color::Red }
};


/**
 * Convenience function for setting a resource trend.
 */
static ResourceTrend compareResources(int src, int dst)
{
	if (src > dst) { return RESOURCE_TREND_UP; }
	if (src < dst) { return RESOURCE_TREND_DOWN; }
	return RESOURCE_TREND_NONE;
}


namespace {
	std::string formatDiff(int diff)
	{
		return ((diff >= 0) ? "+" : "-") + std::to_string(diff);
	}
}


ResourceBreakdownPanel::ResourceBreakdownPanel() : mIcons("ui/icons.png")
{
	size(270, 80);

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
	COMMON_METALS = compareResources(mPlayerResources->commonMetals(), mPreviousResources.commonMetals());
	COMMON_MINERALS = compareResources(mPlayerResources->commonMinerals(), mPreviousResources.commonMinerals());
	RARE_METALS = compareResources(mPlayerResources->rareMetals(), mPreviousResources.rareMetals());
	RARE_MINERALS = compareResources(mPlayerResources->rareMinerals(), mPreviousResources.rareMinerals());

	COMMON_MET_COL = TEXT_COLOR[COMMON_METALS];
	COMMON_MIN_COL = TEXT_COLOR[COMMON_MINERALS];
	RARE_MET_COL = TEXT_COLOR[RARE_METALS];
	RARE_MIN_COL = TEXT_COLOR[RARE_MINERALS];
}


void ResourceBreakdownPanel::update()
{
	Renderer& renderer = Utility<Renderer>::get();
	renderer.drawImageRect(rect(), mSkin);

	const auto commonMetalImageRect = NAS2D::Rectangle{64, 16, 16, 16};
	const auto rareMetalImageRect = NAS2D::Rectangle{80, 16, 16, 16};
	const auto commonMineralImageRect = NAS2D::Rectangle{96, 16, 16, 16};
	const auto rareMineralImageRect = NAS2D::Rectangle{112, 16, 16, 16};

	const std::array resources{
		std::tuple{commonMetalImageRect, "Common Metals", mPlayerResources->commonMetals(), mPreviousResources.commonMetals(), COMMON_METALS, COMMON_MET_COL},
		std::tuple{rareMetalImageRect, "Rare Metals", mPlayerResources->rareMetals(), mPreviousResources.rareMetals(), RARE_METALS, COMMON_MIN_COL},
		std::tuple{commonMineralImageRect, "Common Minerals", mPlayerResources->commonMinerals(), mPreviousResources.commonMinerals(), COMMON_MINERALS, RARE_MET_COL},
		std::tuple{rareMineralImageRect, "Rare Minerals", mPlayerResources->rareMinerals(), mPreviousResources.rareMinerals(), RARE_MINERALS, RARE_MIN_COL},
	};

	auto position = rect().startPoint() + NAS2D::Vector{5, 5};
	for (const auto& [imageRect, text, value, oldValue, resourceTrend, valueChangeColor] : resources)
	{
		renderer.drawSubImage(mIcons, position, imageRect);
		renderer.drawText(*FONT, text, position + NAS2D::Vector{23, 0}, NAS2D::Color::White);
		const auto valueString = std::to_string(value);
		renderer.drawText(*FONT, valueString, position + NAS2D::Vector{195 - FONT->width(valueString), 0}, NAS2D::Color::White);
		const auto changeIconImageRect = NAS2D::Rectangle<int>::Create(ICON_SLICE[resourceTrend], NAS2D::Vector{8, 8});
		renderer.drawSubImage(mIcons, position + NAS2D::Vector{215, 3}, changeIconImageRect);
		renderer.drawText(*FONT, formatDiff(value - oldValue), position + NAS2D::Vector{235, 0}, valueChangeColor);
		position.y() += 18;
	}
}
