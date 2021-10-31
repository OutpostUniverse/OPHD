// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"

#include "Route.h"

#include "../Constants/UiConstants.h"
#include "../Common.h"
#include "../Cache.h"
#include "../Mine.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>
#include <vector>
#include <algorithm>

extern NAS2D::Point<int> MOUSE_COORDS;

namespace
{
	std::map<int, std::string> LevelStringTable =
	{
		{constants::DepthSurface, constants::LevelSurface},
		{constants::DepthUnderground1, constants::Levelunderground1},
		{constants::DepthUnderground2, constants::Levelunderground2},
		{constants::DepthUnderground3, constants::Levelunderground3},
		{constants::DepthUnderground4, constants::Levelunderground4}
	};
}


void MapViewState::drawSystemButton() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	auto position = NAS2D::Point{renderer.size().x - 80, constants::MarginTight};
	constexpr auto textOffset = NAS2D::Vector{constants::ResourceIconSize + constants::Margin, 3 - constants::MarginTight};

	// Turns
	const auto turnImageRect = NAS2D::Rectangle{128, 0, constants::ResourceIconSize, constants::ResourceIconSize};
	renderer.drawSubImage(mUiIcons, position, turnImageRect);
	const auto& font = fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);
	renderer.drawText(font, std::to_string(mTurnCount), position + textOffset, NAS2D::Color::White);

	position = mTooltipSystemButton.rect().startPoint() + NAS2D::Vector{constants::MarginTight, constants::MarginTight};
	bool isMouseInMenu = mTooltipSystemButton.rect().contains(MOUSE_COORDS);
	int menuGearHighlightOffsetX = isMouseInMenu ? 144 : 128;
	const auto menuImageRect = NAS2D::Rectangle{menuGearHighlightOffsetX, 32, constants::ResourceIconSize, constants::ResourceIconSize};
	renderer.drawSubImage(mUiIcons, position, menuImageRect);
}


/**
 * Draws navigation UI.
 */
void MapViewState::drawNavInfo() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	const std::array buttonDrawAreas
	{
		std::tuple{mMoveDownIconRect, NAS2D::Rectangle{64, 128, 32, 32}},
		std::tuple{mMoveUpIconRect, NAS2D::Rectangle{96, 128, 32, 32}},
		std::tuple{mMoveEastIconRect, NAS2D::Rectangle{32, 128, 32, 16}},
		std::tuple{mMoveWestIconRect, NAS2D::Rectangle{32, 144, 32, 16}},
		std::tuple{mMoveNorthIconRect, NAS2D::Rectangle{0, 128, 32, 16}},
		std::tuple{mMoveSouthIconRect, NAS2D::Rectangle{0, 144, 32, 16}},
	};
	for (const auto& [currentIconBounds, subImageBounds] : buttonDrawAreas)
	{
		bool isMouseInIcon = currentIconBounds.contains(MOUSE_COORDS);
		NAS2D::Color color = isMouseInIcon ? NAS2D::Color::Red : NAS2D::Color::White;
		renderer.drawSubImage(mUiIcons, currentIconBounds.startPoint(), subImageBounds, color);
	}

	// Display the levels "bar"
	const auto& font = fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);
	const auto stepSizeWidth = font.width("IX");
	auto position = NAS2D::Point{renderer.size().x - 5, mMiniMapBoundingBox.y - 30};
	for (int i = mTileMap->maxDepth(); i >= 0; i--)
	{
		const auto levelString = (i == 0) ? std::string{"S"} : std::to_string(i);
		const auto textSize = font.size(levelString);
		bool isCurrentDepth = i == mTileMap->currentDepth();
		NAS2D::Color color = isCurrentDepth ? NAS2D::Color::Red : NAS2D::Color{200, 200, 200};
		renderer.drawText(font, levelString, position - textSize, color);
		position.x -= stepSizeWidth;
	}

	// Explicit current level
	const auto& currentLevelString = LevelStringTable[mTileMap->currentDepth()];
	const auto& fontBoldMedium = fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium);
	const auto currentLevelPosition = mMiniMapBoundingBox.crossXPoint() - fontBoldMedium.size(currentLevelString) - NAS2D::Vector{0, 12};
	renderer.drawText(fontBoldMedium, currentLevelString, currentLevelPosition, NAS2D::Color::White);
}
