// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ResourceBreakdownPanel.h"

#include "../Constants.h"
#include "../FontManager.h"

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


std::map<ResourceTrend, Point_2df> ICON_SLICE
{
	{ RESOURCE_TREND_NONE,	Point_2df(16.0f, 64.0f) },
	{ RESOURCE_TREND_UP,	Point_2df(8.0f, 64.0f) },
	{ RESOURCE_TREND_DOWN,	Point_2df(0.0f, 64.0f) }
};


std::map<ResourceTrend, Color> TEXT_COLOR
{
	{ RESOURCE_TREND_NONE,	Color(255, 255, 255, 255) },
	{ RESOURCE_TREND_UP,	Color(0, 185, 0, 255) },
	{ RESOURCE_TREND_DOWN,	Color(255, 0, 0, 255) }
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
	Renderer& r = Utility<Renderer>::get();
	r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkin);

	r.drawSubImage(mIcons, 5.0f, rect().y() + 5.0f, 64.0f, 16.0f, 16.0f, 16.0f);
	r.drawSubImage(mIcons, 5.0f, rect().y() + 23.0f, 80.0f, 16.0f, 16.0f, 16.0f);
	r.drawSubImage(mIcons, 5.0f, rect().y() + 41.0f, 96.0f, 16.0f, 16.0f, 16.0f);
	r.drawSubImage(mIcons, 5.0f, rect().y() + 59.0f, 112.0f, 16.0f, 16.0f, 16.0f);

	r.drawText(*FONT, "Common Metals",		28.0f, rect().y() + 5.0f, 255, 255, 255);
	r.drawText(*FONT, "Rare Metals",		28.0f, rect().y() + 23.0f, 255, 255, 255);
	r.drawText(*FONT, "Common Minerals",	28.0f, rect().y() + 41.0f, 255, 255, 255);
	r.drawText(*FONT, "Rare Minerals",		28.0f, rect().y() + 59.0f, 255, 255, 255);

	std::string fmt = string_format("%i", mPlayerResources->commonMetals());
	r.drawText(*FONT, fmt, 200.0f - FONT->width(fmt) , rect().y() + 5.0f, 255, 255, 255);
	
	fmt = string_format("%i", mPlayerResources->commonMinerals());
	r.drawText(*FONT, fmt, 200.0f - FONT->width(fmt), rect().y() + 23.0f, 255, 255, 255);

	fmt = string_format("%i", mPlayerResources->rareMetals());
	r.drawText(*FONT, fmt, 200.0f - FONT->width(fmt), rect().y() + 41.0f, 255, 255, 255);

	fmt = string_format("%i", mPlayerResources->rareMinerals());
	r.drawText(*FONT, fmt, 200.0f - FONT->width(fmt), rect().y() + 59.0f, 255, 255, 255);

	r.drawSubImage(mIcons, 220.0f, rect().y() + 8.0f,	ICON_SLICE[COMMON_METALS].x(),		ICON_SLICE[COMMON_METALS].y(),		8.0f, 8.0f);
	r.drawSubImage(mIcons, 220.0f, rect().y() + 26.0f,	ICON_SLICE[COMMON_MINERALS].x(),	ICON_SLICE[COMMON_MINERALS].y(),	8.0f, 8.0f);
	r.drawSubImage(mIcons, 220.0f, rect().y() + 44.0f,	ICON_SLICE[RARE_METALS].x(),		ICON_SLICE[RARE_METALS].y(),		8.0f, 8.0f);
	r.drawSubImage(mIcons, 220.0f, rect().y() + 62.0f,	ICON_SLICE[RARE_MINERALS].x(),		ICON_SLICE[RARE_MINERALS].y(),		8.0f, 8.0f);


	fmt = string_format("%+i", mPlayerResources->commonMetals() - mPreviousResources.commonMetals());
	r.drawText(*FONT, fmt, 235.0f, rect().y() + 5.0f, COMMON_MET_COL.red(), COMMON_MET_COL.green(), COMMON_MET_COL.blue());

	fmt = string_format("%+i", mPlayerResources->commonMinerals() - mPreviousResources.commonMinerals());
	r.drawText(*FONT, fmt, 235.0f, rect().y() + 23.0f, COMMON_MIN_COL.red(), COMMON_MIN_COL.green(), COMMON_MIN_COL.blue());

	fmt = string_format("%+i", mPlayerResources->rareMetals() - mPreviousResources.rareMetals());
	r.drawText(*FONT, fmt, 235.0f, rect().y() + 41.0f, RARE_MET_COL.red(), RARE_MET_COL.green(), RARE_MET_COL.blue());

	fmt = string_format("%+i", mPlayerResources->rareMinerals() - mPreviousResources.rareMinerals());
	r.drawText(*FONT, fmt, 235.0f, rect().y() + 59.0f, RARE_MIN_COL.red(), RARE_MIN_COL.green(), RARE_MIN_COL.blue());
}
