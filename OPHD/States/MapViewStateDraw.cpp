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
	std::map<int, std::string> LEVEL_STRING_TABLE =
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


bool MapViewState::drawNavIcon(NAS2D::Renderer& renderer, const NAS2D::Rectangle<int>& currentIconBounds, const NAS2D::Rectangle<int>& subImageBounds, const NAS2D::Color& iconColor, const NAS2D::Color& iconHighlightColor) const
{
	bool isMouseInIcon = currentIconBounds.contains(MOUSE_COORDS);
	NAS2D::Color color = isMouseInIcon ? iconHighlightColor : iconColor;
	renderer.drawSubImage(mUiIcons, currentIconBounds.startPoint(), subImageBounds, color);
	return isMouseInIcon;
}

/**
 * Draws navigation UI.
 */
void MapViewState::drawNavInfo() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	drawNavIcon(renderer, mMoveDownIconRect, NAS2D::Rectangle{64, 128, 32, 32}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, mMoveUpIconRect, NAS2D::Rectangle{96, 128, 32, 32}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, mMoveEastIconRect, NAS2D::Rectangle{32, 128, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, mMoveWestIconRect, NAS2D::Rectangle{32, 144, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, mMoveNorthIconRect, NAS2D::Rectangle{0, 128, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, mMoveSouthIconRect, NAS2D::Rectangle{0, 144, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);

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
	const auto& currentLevelString = LEVEL_STRING_TABLE[mTileMap->currentDepth()];
	const auto& fontBoldMedium = fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FontPrimaryMedium);
	const auto currentLevelPosition = mMiniMapBoundingBox.crossXPoint() - fontBoldMedium.size(currentLevelString) - NAS2D::Vector{0, 12};
	renderer.drawText(fontBoldMedium, currentLevelString, currentLevelPosition, NAS2D::Color::White);
}
