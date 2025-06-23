// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"

#include "../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>


extern NAS2D::Point<int> MOUSE_COORDS;


void MapViewState::drawSystemButton() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	auto position = NAS2D::Point{renderer.size().x - 80, constants::MarginTight};
	constexpr auto textOffset = NAS2D::Vector{constants::ResourceIconSize + constants::Margin, 3 - constants::MarginTight};

	const auto drawIcon = [&renderer, &icons = mUiIcons, height = mResourceInfoBar.size().y](NAS2D::Point<int> iconPosition, NAS2D::Rectangle<int> imageRect)
	{
		const auto imageOffset = NAS2D::Vector{0, (height - imageRect.size.y - 1) / 2};
		renderer.drawSubImage(icons, iconPosition + imageOffset, imageRect);
	};

	// Turns
	const auto turnImageRect = NAS2D::Rectangle<int>{{128, 0}, {constants::ResourceIconSize, constants::ResourceIconSize}};
	drawIcon(position, turnImageRect);
	const auto& font = Control::getDefaultFont();
	renderer.drawText(font, std::to_string(mTurnCount), position + textOffset, NAS2D::Color::White);

	position = mTooltipSystemButton.area().position + NAS2D::Vector{constants::MarginTight, constants::MarginTight};
	bool isMouseInMenu = mTooltipSystemButton.area().contains(MOUSE_COORDS);
	int menuGearHighlightOffsetX = isMouseInMenu ? 144 : 128;
	const auto menuImageRect = NAS2D::Rectangle<int>{{menuGearHighlightOffsetX, 32}, {constants::ResourceIconSize, constants::ResourceIconSize}};
	drawIcon(position, menuImageRect);
}
