#include "PopulationPanel.h"

#include "../Cache.h"
#include "../Common.h"
#include "../Constants.h"
#include "../Population/Population.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>


using namespace NAS2D;


static const auto formatDiff = [](int diff)
{
	return ((diff > 0) ? "+" : "") + std::to_string(diff);
};

static const auto trendIndex = [](int value)
{
	return (value >= 0) ? 0 : 1;
};

static const std::array trend
{
	Color{ 0, 185, 0 },
	Color::Red
};


static const auto moraleIndex = [](int val)
{
	return std::clamp(val, 0, 999) / 200;
};


static const std::array moraleStringColor
{
	Color::Red,
	Color::Orange,
	Color::Yellow,
	Color::White,
	Color{ 0, 185, 0 }
};


PopulationPanel::PopulationPanel() :
	mFont{ fontCache.load(constants::FONT_PRIMARY, 14) },
	mFontBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, 14) },
	mIcons{ imageCache.load("ui/icons.png") },
	mSkin
	{
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
	constexpr int linesOfText = 15;
	constexpr int edgeBuffer = constants::MARGIN * 2;
	
	const int windowHeight = mFont.height() * linesOfText + edgeBuffer;

	int largestStringLength = 0;
	for (int i = 0; i < moraleStringTableCount(); ++i)
	{
		const int legnthCompare = mFont.width(moraleString(i));
		if (legnthCompare > largestStringLength)
		{
			largestStringLength = legnthCompare;
		}
	}

	largestStringLength += (edgeBuffer) * 2 + mFont.width("999999");

	mPopulationPanelWidth = mFont.width(constants::PopulationBreakdown) + edgeBuffer;
	const int windowWidth = largestStringLength + edgeBuffer + mPopulationPanelWidth;


	size({ windowWidth, windowHeight });
}


void PopulationPanel::update()
{
	auto& renderer = Utility<Renderer>::get();
	mSkin.draw(renderer, mRect);

	auto position = NAS2D::Point{ positionX() + constants::MARGIN, positionY() + constants::MARGIN };

	// POPULATION
	renderer.drawText(mFontBold, constants::PopulationBreakdown, position);
	const std::array populationData
	{
		std::pair{NAS2D::Rectangle{0, 96, 32, 32}, mPopulation->size(Population::PersonRole::ROLE_CHILD)},
		std::pair{NAS2D::Rectangle{32, 96, 32, 32}, mPopulation->size(Population::PersonRole::ROLE_STUDENT)},
		std::pair{NAS2D::Rectangle{64, 96, 32, 32}, mPopulation->size(Population::PersonRole::ROLE_WORKER)},
		std::pair{NAS2D::Rectangle{96, 96, 32, 32}, mPopulation->size(Population::PersonRole::ROLE_SCIENTIST)},
		std::pair{NAS2D::Rectangle{128, 96, 32, 32}, mPopulation->size(Population::PersonRole::ROLE_RETIRED)},
	};

	position.y += 15;
	const auto fontOffset = NAS2D::Vector{ 40, 16 - (mFont.height() / 2) };
	for (const auto& [imageRect, personCount] : populationData)
	{
		renderer.drawSubImage(mIcons, position, imageRect);
		renderer.drawText(mFont, std::to_string(personCount), position + fontOffset, { 0, 185, 0 });
		position.y += 34;
	}

	const int fontHeight = mFont.height();


	// MORALE
	position = NAS2D::Point{ positionX() + mPopulationPanelWidth, positionY() + constants::MARGIN };
	renderer.drawLine(position, position + NAS2D::Vector<int>{ 0, rect().height - 10 }, Color::DarkGray);

	position.x += constants::MARGIN;
	renderer.drawText(mFontBold, constants::MoraleBreakdown, position);

	position.y += fontHeight;
	const auto moraleLevel = moraleIndex(mMorale);
	renderer.drawText(mFont, moraleString(Morale::Description) + moraleString(moraleLevel), position, moraleStringColor[moraleLevel]);

	position.y += fontHeight;
	renderer.drawText(mFont, "Current: " + std::to_string(mMorale) + " / Previous: " + std::to_string(mPreviousMorale), position);

	position.y += fontHeight;
	int capacityPercent = (mResidentialCapacity > 0) ? (mPopulation->size() * 100 / mResidentialCapacity) : 0;
	const auto housingText = "Housing: " + std::to_string(mPopulation->size()) + " / " + std::to_string(mResidentialCapacity) + "  (" + std::to_string(capacityPercent) + "%)";
	renderer.drawText(mFont, housingText, position, NAS2D::Color::White);

	position.y += fontHeight + fontHeight / 2;

	renderer.drawLine(position, position + NAS2D::Vector<int>{ rect().width - 160, 0 }, Color::DarkGray);

	position.y += fontHeight / 2;

	
	for (auto& item : mMoraleChangeReasons)
	{
		renderer.drawText(mFont, item.first, position);

		const auto text = formatDiff(item.second);
		const NAS2D::Point<int> labelPosition = { rect().x + rect().width - mFont.width(text) - 5 , position.y };

		renderer.drawText(mFont, text, labelPosition, trend[trendIndex(item.second)]);
		position.y += fontHeight;
	}
	

	/*
	for (size_t i = 0; i < 10; ++i)
	{
		renderer.drawText(mFont, "Some Line of Text........", position);
		position.y += fontHeight;
	}
	*/
}
