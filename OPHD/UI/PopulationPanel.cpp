#include "PopulationPanel.h"

#include "../Cache.h"
#include "../Constants.h"
#include "../Population/Population.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


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
}
