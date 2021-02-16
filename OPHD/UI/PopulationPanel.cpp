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
	mFont{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL) },
	mFontBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL) },
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
	size({ 320, 195 });
}


void PopulationPanel::update()
{
	auto& renderer = Utility<Renderer>::get();
	mSkin.draw(renderer, mRect);

	/*
	auto position = NAS2D::Point{positionX() + 5, positionY() + 5};
	renderer.drawText(mFont, "Morale: " + std::to_string(*mMorale), position, NAS2D::Color::White);
	position.y += 10;
	renderer.drawText(mFont, "Previous: " + std::to_string(*mPreviousMorale), position, NAS2D::Color::White);

	mCapacity = (mResidentialCapacity > 0) ? (mPopulation->size() * 100 / mResidentialCapacity) : 0;

	position.y += 15;
	const auto text = "Housing: " + std::to_string(mPopulation->size()) + " / " + std::to_string(mResidentialCapacity) + "  (" + std::to_string(mCapacity) + "%)";
	renderer.drawText(mFont, text, position, NAS2D::Color::White);
	*/

	auto position = NAS2D::Point{ positionX() + 5, positionY() + 5 };

	// POPULATION
	renderer.drawText(mFontBold, "Population Breakdown", position);
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

	// MORALE
	position = NAS2D::Point{ positionX() + 140, positionY() + 5 };
	renderer.drawLine(position, position + NAS2D::Vector<int>{ 0, 185 });

	position.x += 5;
	renderer.drawText(mFontBold, "Morale Breakdown", position);

	position.y += 15;
	const auto moraleLevel = moraleIndex(mMorale);
	renderer.drawText(mFont, constants::MoraleDescription + moraleDescription(moraleLevel), position, moraleStringColor[moraleLevel]);

	position.y += 13;
	renderer.drawText(mFont, "Current: " + std::to_string(mMorale) + " / Previous: " + std::to_string(mPreviousMorale), position);

	position.y += 27;

	for (auto& item : mMoraleChangeReasons)
	{
		renderer.drawText(mFont, item.first, position);
		position.x += 140;

		const std::string text = formatDiff(item.second);
		const NAS2D::Point<int> labelPosition = { rect().x + rect().width - mFont.width(text) - 5 , position.y };

		renderer.drawText(mFont, text, labelPosition, trend[trendIndex(item.second)]);
		position += NAS2D::Vector{ -140, 13 };
	}
}
