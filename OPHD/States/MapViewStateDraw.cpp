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
extern const NAS2D::Font* MAIN_FONT; /// yuck


/**
 * Draws the minimap and all icons/overlays for it.
 */
void MapViewState::drawMiniMap() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto miniMapBoxFloat = mMiniMapBoundingBox.to<float>();
	renderer.clipRect(miniMapBoxFloat);

	bool isHeightmapToggled = mBtnToggleHeightmap.toggled();
	renderer.drawImage(*(isHeightmapToggled ? mHeightMap : mMapDisplay).get(), miniMapBoxFloat.startPoint());

	const auto miniMapOffset = mMiniMapBoundingBox.startPoint() - NAS2D::Point{0, 0};
	const auto ccPosition = ccLocation();
	if (ccPosition != CcNotPlaced)
	{
		const auto ccOffsetPosition = ccPosition + miniMapOffset;
		const auto ccCommRangeImageRect = NAS2D::Rectangle{166, 226, 30, 30};
		renderer.drawSubImage(mUiIcons, ccOffsetPosition - ccCommRangeImageRect.size() / 2, ccCommRangeImageRect);
		renderer.drawBoxFilled(NAS2D::Rectangle<int>::Create(ccOffsetPosition - NAS2D::Vector{1, 1}, NAS2D::Vector{3, 3}), NAS2D::Color::White);
	}

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	for (auto commTower : structureManager.getStructures<CommTower>())
	{
		if (commTower->operational())
		{
			const auto commTowerPosition = structureManager.tileFromStructure(commTower).xy();
			const auto commTowerRangeImageRect = NAS2D::Rectangle{146, 236, 20, 20};
			renderer.drawSubImage(mUiIcons, commTowerPosition + miniMapOffset - commTowerRangeImageRect.size() / 2, commTowerRangeImageRect);
		}
	}

	for (auto minePosition : mTileMap->mineLocations())
	{
		Mine* mine = mTileMap->getTile({minePosition, 0}).mine();
		if (!mine) { break; } // avoids potential race condition where a mine is destroyed during an updated cycle.

		auto mineBeaconStatusOffsetX = 0;
		if (!mine->active()) { mineBeaconStatusOffsetX = 0; }
		else if (!mine->exhausted()) { mineBeaconStatusOffsetX = 8; }
		else { mineBeaconStatusOffsetX = 16; }

		const auto mineImageRect = NAS2D::Rectangle{mineBeaconStatusOffsetX, 0, 7, 7};
		renderer.drawSubImage(mUiIcons, minePosition + miniMapOffset - NAS2D::Vector{2, 2}, mineImageRect);
	}

	// Temporary debug aid, will be slow with high numbers of mines
	// especially with routes of longer lengths.
	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	for (auto route : routeTable)
	{
		for (auto tile : route.second.path)
		{
			const auto tilePosition = static_cast<Tile*>(tile)->xy();
			renderer.drawPoint(tilePosition + miniMapOffset, NAS2D::Color::Magenta);
		}
	}

	for (auto robotEntry : mRobotList)
	{
		const auto robotPosition = robotEntry.second->xy();
		renderer.drawPoint(robotPosition + miniMapOffset, NAS2D::Color::Cyan);
	}

	const auto& viewLocation = mTileMap->mapViewLocation();
	const auto edgeLength = mTileMap->edgeLength();
	const auto viewBoxSize = NAS2D::Vector{edgeLength, edgeLength};
	const auto viewBoxPosition = viewLocation + miniMapOffset;

	renderer.drawBox(NAS2D::Rectangle<int>::Create(viewBoxPosition + NAS2D::Vector{1, 1}, viewBoxSize), NAS2D::Color{0, 0, 0, 180});
	renderer.drawBox(NAS2D::Rectangle<int>::Create(viewBoxPosition, viewBoxSize), NAS2D::Color::White);

	renderer.clipRectClear();
}


void MapViewState::drawSystemButton() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	auto position = NAS2D::Point{renderer.size().x - 80, constants::MarginTight};
	constexpr auto textOffset = NAS2D::Vector{constants::ResourceIconSize + constants::Margin, 3 - constants::MarginTight};

	// Turns
	const auto turnImageRect = NAS2D::Rectangle{128, 0, constants::ResourceIconSize, constants::ResourceIconSize};
	renderer.drawSubImage(mUiIcons, position, turnImageRect);
	renderer.drawText(*MAIN_FONT, std::to_string(mTurnCount), position + textOffset, NAS2D::Color::White);

	position = mTooltipSystemButton.rect().startPoint() + NAS2D::Vector{constants::MarginTight, constants::MarginTight};
	bool isMouseInMenu = mTooltipSystemButton.rect().contains(MOUSE_COORDS);
	int menuGearHighlightOffsetX = isMouseInMenu ? 144 : 128;
	const auto menuImageRect = NAS2D::Rectangle{menuGearHighlightOffsetX, 32, constants::ResourceIconSize, constants::ResourceIconSize};
	renderer.drawSubImage(mUiIcons, position, menuImageRect);
}


/**
 * Draws robot deployment information.
 */
void MapViewState::drawRobotInfo() const
{
	if (ccLocation() == CcNotPlaced) { return; }

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// Robots: Miner (last one), Dozer (middle one), Digger (first one)
	// Start from the bottom - The bottom UI Height - Icons Height - 8 (1 offset to avoid the last to be glued with at the border)
	auto position = NAS2D::Point{8, renderer.size().y - constants::BottomUiHeight - 25 - 8};
	constexpr auto textOffset = NAS2D::Vector{30, 7};

	const auto minerImageRect = NAS2D::Rectangle{231, 18, 25, 25};
	const auto dozerImageRect = NAS2D::Rectangle{206, 18, 25, 25};
	const auto diggerImageRect = NAS2D::Rectangle{181, 18, 25, 25};
	const auto robotSummaryImageRect = NAS2D::Rectangle{231, 43, 25, 25};

	const std::array icons{
		std::tuple{minerImageRect, mRobotPool.getAvailableCount(Robot::Type::Miner), mRobotPool.miners().size()},
		std::tuple{dozerImageRect, mRobotPool.getAvailableCount(Robot::Type::Dozer), mRobotPool.dozers().size()},
		std::tuple{diggerImageRect, mRobotPool.getAvailableCount(Robot::Type::Digger), mRobotPool.diggers().size()},
		std::tuple{robotSummaryImageRect, static_cast<int>(mRobotPool.currentControlCount()), static_cast<std::size_t>(mRobotPool.robotControlMax())},
	};

	for (const auto& [imageRect, parts, total] : icons)
	{
		renderer.drawSubImage(mUiIcons, position, imageRect);
		const auto text = std::to_string(parts) + "/" + std::to_string(total);
		renderer.drawText(*MAIN_FONT, text, position + textOffset, NAS2D::Color::White);
		position.y -= 25;
	}
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
	const auto stepSizeWidth = MAIN_FONT->width("IX");
	auto position = NAS2D::Point{renderer.size().x - 5, mMiniMapBoundingBox.y - 30};
	for (int i = mTileMap->maxDepth(); i >= 0; i--)
	{
		const auto levelString = (i == 0) ? std::string{"S"} : std::to_string(i);
		const auto textSize = MAIN_FONT->size(levelString);
		bool isCurrentDepth = i == mTileMap->currentDepth();
		NAS2D::Color color = isCurrentDepth ? NAS2D::Color::Red : NAS2D::Color{200, 200, 200};
		renderer.drawText(*MAIN_FONT, levelString, position - textSize, color);
		position.x -= stepSizeWidth;
	}
}
