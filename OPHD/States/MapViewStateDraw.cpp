// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"

#include "Route.h"

#include "../Constants/UiConstants.h"
#include "../Common.h"
#include "../Cache.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"
#include "../MapObjects/Mine.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>
#include <vector>
#include <algorithm>

extern NAS2D::Point<int> MOUSE_COORDS;


void MapViewState::drawSystemButton() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	auto position = NAS2D::Point{renderer.size().x - 80, constants::MarginTight};
	constexpr auto textOffset = NAS2D::Vector{constants::ResourceIconSize + constants::Margin, 3 - constants::MarginTight};

	// Turns
	const auto turnImageRect = NAS2D::Rectangle<int>{{128, 0}, {constants::ResourceIconSize, constants::ResourceIconSize}};
	renderer.drawSubImage(mUiIcons, position, turnImageRect);
	const auto& font = fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);
	renderer.drawText(font, std::to_string(mTurnCount), position + textOffset, NAS2D::Color::White);

	position = mTooltipSystemButton.rect().position + NAS2D::Vector{constants::MarginTight, constants::MarginTight};
	bool isMouseInMenu = mTooltipSystemButton.rect().contains(MOUSE_COORDS);
	int menuGearHighlightOffsetX = isMouseInMenu ? 144 : 128;
	const auto menuImageRect = NAS2D::Rectangle<int>{{menuGearHighlightOffsetX, 32}, {constants::ResourceIconSize, constants::ResourceIconSize}};
	renderer.drawSubImage(mUiIcons, position, menuImageRect);
}
