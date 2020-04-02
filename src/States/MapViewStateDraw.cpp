// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"

#include "../Constants.h"
#include "../FontManager.h"

#include <string>
#include <vector>
#include <algorithm>

extern NAS2D::Rectangle_2d MENU_ICON;

extern NAS2D::Rectangle_2d MOVE_NORTH_ICON;
extern NAS2D::Rectangle_2d MOVE_SOUTH_ICON;
extern NAS2D::Rectangle_2d MOVE_EAST_ICON;
extern NAS2D::Rectangle_2d MOVE_WEST_ICON;
extern NAS2D::Rectangle_2d MOVE_UP_ICON;
extern NAS2D::Rectangle_2d MOVE_DOWN_ICON;

extern NAS2D::Point_2d MOUSE_COORDS;

extern NAS2D::Font* MAIN_FONT; /// yuck
extern std::vector<void*> path;


namespace {
	uint8_t calcGlowIntensity()
	{
		static NAS2D::Timer glowTimer;
		static int glowStepDelta = 20;
		static int glowStep = 0;

		if (glowTimer.accumulator() >= 10)
		{
			glowTimer.reset();

			glowStep += glowStepDelta;
			if (glowStep >= 255)
			{
				glowStep = 255;
				glowStepDelta = -glowStepDelta;
			}
			else if (glowStep <= 0)
			{
				glowStep = 0;
				glowStepDelta = -glowStepDelta;
			}
		}
		return static_cast<uint8_t>(glowStep);
	}
}


/**
 * Draws the minimap and all icons/overlays for it.
 */
void MapViewState::drawMiniMap()
{
	NAS2D::Renderer& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto miniMapBoxFloat = mMiniMapBoundingBox.to<float>();
	renderer.clipRect(miniMapBoxFloat);

	bool isHeightmapToggled = mBtnToggleHeightmap.toggled();
	renderer.drawImage(isHeightmapToggled ? mHeightMap : mMapDisplay, miniMapBoxFloat.startPoint());

	const auto miniMapOffset = mMiniMapBoundingBox.startPoint() - NAS2D::Point<int>{0, 0};
	const auto ccPosition = ccLocation();
	if (ccPosition != NAS2D::Point<int>{0, 0})
	{
		const auto ccOffsetPosition = ccPosition + miniMapOffset;
		const auto ccCommRangeImageRect = NAS2D::Rectangle<int>{166, 226, 30, 30};
		renderer.drawSubImage(mUiIcons, ccOffsetPosition - ccCommRangeImageRect.size() / 2, ccCommRangeImageRect);
		renderer.drawBoxFilled(NAS2D::Rectangle<int>::Create(ccOffsetPosition - NAS2D::Vector<int>{1, 1}, NAS2D::Vector<int>{3, 3}), NAS2D::Color::White);
	}

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	for (auto commTower : structureManager.structureList(Structure::CLASS_COMM))
	{
		if (commTower->operational())
		{
			const auto commTowerPosition = structureManager.tileFromStructure(commTower)->position();
			const auto commTowerRangeImageRect = NAS2D::Rectangle<int>{146, 236, 20, 20};
			renderer.drawSubImage(mUiIcons, commTowerPosition + miniMapOffset - commTowerRangeImageRect.size() / 2, commTowerRangeImageRect);
		}
	}

	for (auto minePosition : mTileMap->mineLocations())
	{
		Mine* mine = mTileMap->getTile(minePosition, 0)->mine();
		if (!mine) { break; } // avoids potential race condition where a mine is destroyed during an updated cycle.

		auto mineBeaconStatusOffsetX = 0;
		if (!mine->active()) { mineBeaconStatusOffsetX = 0; }
		else if (!mine->exhausted()) { mineBeaconStatusOffsetX = 8; }
		else { mineBeaconStatusOffsetX = 16; }

		const auto mineImageRect = NAS2D::Rectangle<int>{mineBeaconStatusOffsetX, 0, 7, 7};
		renderer.drawSubImage(mUiIcons, minePosition + miniMapOffset - NAS2D::Vector<int>{2, 2}, mineImageRect);
	}

	for (auto tile : path)
	{
		const auto tilePosition = static_cast<Tile*>(tile)->position();
		renderer.drawPoint(tilePosition + miniMapOffset, NAS2D::Color::Magenta);
	}

	for (auto robotEntry : mRobotList)
	{
		const auto robotPosition = robotEntry.second->position();
		renderer.drawPoint(robotPosition + miniMapOffset, NAS2D::Color::Cyan);
	}

	const auto& viewLocation = mTileMap->mapViewLocation();
	const auto edgeLength = mTileMap->edgeLength();
	const auto viewBoxSize = NAS2D::Vector<int>{edgeLength, edgeLength};
	const auto viewBoxPosition = viewLocation + miniMapOffset;

	renderer.drawBox(NAS2D::Rectangle<int>::Create(viewBoxPosition + NAS2D::Vector<int>{1, 1}, viewBoxSize), NAS2D::Color{0, 0, 0, 180});
	renderer.drawBox(NAS2D::Rectangle<int>::Create(viewBoxPosition, viewBoxSize), NAS2D::Color::White);

	renderer.clipRectClear();
}


/**
 * Draws the resource information bar.
 */
void MapViewState::drawResourceInfo()
{
	NAS2D::Renderer& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.drawBoxFilled(NAS2D::Rectangle<float>{0, 0, renderer.width(), constants::RESOURCE_ICON_SIZE + 4}, NAS2D::Color{39, 39, 39});
	renderer.drawBox(NAS2D::Rectangle<float>{0, 0, renderer.width(), constants::RESOURCE_ICON_SIZE + 4}, NAS2D::Color{21, 21, 21});
	renderer.drawLine(NAS2D::Point<float>{1, 0}, NAS2D::Point<float>{renderer.width() - 2, 0}, NAS2D::Color{56, 56, 56});

	// Resources
	int x = constants::MARGIN_TIGHT + 12;
	int offsetX = constants::RESOURCE_ICON_SIZE + 40;
	auto position = NAS2D::Point<int>{constants::MARGIN_TIGHT + 12, constants::MARGIN_TIGHT};
	constexpr auto textOffset = NAS2D::Vector<int>{constants::RESOURCE_ICON_SIZE + constants::MARGIN, 3 - constants::MARGIN_TIGHT};

	const auto unpinnedImageRect = NAS2D::Rectangle<int>{0, 72, 8, 8};
	const auto pinnedImageRect = NAS2D::Rectangle<int>{8, 72, 8, 8};

	renderer.drawSubImage(mUiIcons, NAS2D::Point{2, 7}, mPinResourcePanel ? unpinnedImageRect : pinnedImageRect);
	renderer.drawSubImage(mUiIcons, NAS2D::Point{675, 7}, mPinPopulationPanel ? unpinnedImageRect : pinnedImageRect);

	const auto glowIntensity = calcGlowIntensity();
	const auto glowColor = NAS2D::Color{255, glowIntensity, glowIntensity};

	// Common Metals
	const auto commonMetalsImageRect = NAS2D::Rectangle<int>{64, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, commonMetalsImageRect);
	bool shouldCommonMetalsGlow = mPlayerResources.commonMetals() <= 10;
	auto color = shouldCommonMetalsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.commonMetals()), position + textOffset, color);

	// Rare Metals
	position.x() += offsetX;
	const auto rareMetalsImageRect = NAS2D::Rectangle<int>{80, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, rareMetalsImageRect);
	bool shouldRareMetalsGlow = mPlayerResources.rareMetals() <= 10;
	color = shouldRareMetalsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.rareMetals()), position + textOffset, color);

	// Common Minerals
	position.x() += x + offsetX;
	const auto commonMineralsImageRect = NAS2D::Rectangle<int>{96, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, commonMineralsImageRect);
	bool shouldCommonMineralsGlow = mPlayerResources.commonMinerals() <= 10;
	color = shouldCommonMineralsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.commonMinerals()), position + textOffset, color);

	// Rare Minerals
	position.x() += x + offsetX;
	const auto rareMineralsImageRect = NAS2D::Rectangle<int>{112, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, rareMineralsImageRect);
	bool shouldRareMineralsGlow = mPlayerResources.rareMinerals() <= 10;
	color = shouldRareMineralsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.rareMinerals()), position + textOffset, color);

	// Capacity (Storage, Food, Energy)
	constexpr auto iconSize = constants::RESOURCE_ICON_SIZE;
	const std::array storageCapacities{
		std::tuple{NAS2D::Rectangle{96, 32, iconSize, iconSize}, mPlayerResources.currentLevel(), mPlayerResources.capacity(), mPlayerResources.capacity() - mPlayerResources.currentLevel() <= 100},
		std::tuple{NAS2D::Rectangle{64, 32, iconSize, iconSize}, foodInStorage(), foodTotalStorage(), foodInStorage() <= 10},
		std::tuple{NAS2D::Rectangle{80, 32, iconSize, iconSize}, mPlayerResources.energy(), NAS2D::Utility<StructureManager>::get().totalEnergyProduction(), mPlayerResources.energy() <= 5}
	};

	position.x() += x + offsetX;
	for (const auto& [imageRect, parts, total, isHighlighted] : storageCapacities) {
		renderer.drawSubImage(mUiIcons, position, imageRect);
		color = isHighlighted ? glowColor : NAS2D::Color::White;
		renderer.drawText(*MAIN_FONT, NAS2D::string_format("%i/%i", parts, total), position + textOffset, color);
		position.x() += (x + offsetX) * 2;
	}

	// Population / Morale
	position.x() -= 17;
	int popMoraleDeltaImageOffsetX = mCurrentMorale < mPreviousMorale ? 0 : (mCurrentMorale > mPreviousMorale ? 16 : 32);
	const auto popMoraleDirectionImageRect = NAS2D::Rectangle<int>{popMoraleDeltaImageOffsetX, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, popMoraleDirectionImageRect);

	position.x() += 17;
	const auto moraleLevel = (std::clamp(mCurrentMorale, 1, 999) / 200);
	const auto popMoraleImageRect = NAS2D::Rectangle<int>{176 + moraleLevel * constants::RESOURCE_ICON_SIZE, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, popMoraleImageRect);
	renderer.drawText(*MAIN_FONT, std::to_string(mPopulation.size()), position + textOffset, NAS2D::Color::White);

	bool isMouseInPopPanel = NAS2D::Rectangle{675, 1, 75, 19}.contains(MOUSE_COORDS);
	bool shouldShowPopPanel = mPinPopulationPanel || isMouseInPopPanel;
	if(shouldShowPopPanel) { mPopulationPanel.update(); }

	bool isMouseInResourcePanel = NAS2D::Rectangle{0, 1, static_cast<int>(mResourceBreakdownPanel.width()), 19}.contains(MOUSE_COORDS);
	bool shouldShowResourcePanel = mPinResourcePanel || isMouseInResourcePanel;
	if (shouldShowResourcePanel) { mResourceBreakdownPanel.update(); }

	// Turns
	position.x() = static_cast<int>(renderer.width() - 80);
	const auto turnImageRect = NAS2D::Rectangle<int>{128, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, turnImageRect);
	renderer.drawText(*MAIN_FONT, std::to_string(mTurnCount), position + textOffset, NAS2D::Color::White);

	position = MENU_ICON.startPoint() + NAS2D::Vector<int>{constants::MARGIN_TIGHT, constants::MARGIN_TIGHT};
	bool isMouseInMenu = MENU_ICON.contains(MOUSE_COORDS);
	int menuGearHighlightOffsetX = isMouseInMenu ? 144 : 128;
	const auto menuImageRect = NAS2D::Rectangle<int>{menuGearHighlightOffsetX, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE};
	renderer.drawSubImage(mUiIcons, position, menuImageRect);
}


/**
 * Draws robot deployment information.
 */
void MapViewState::drawRobotInfo()
{
	// CC hasn't been placed yet.
	if (ccLocation() == NAS2D::Point<int>{0, 0}) { return; }

	NAS2D::Renderer& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// Robots: Miner (last one), Dozer (middle one), Digger (first one)
	// Start from the bottom - The bottom UI Height - Icons Height - 8 (1 offset to avoid the last to be glued with at the border)
	auto position = NAS2D::Point<int>{8, static_cast<int>(renderer.height()) - constants::BOTTOM_UI_HEIGHT - 25 - 8};
	constexpr auto textOffset = NAS2D::Vector<int>{30, 7};

	const auto minerImageRect = NAS2D::Rectangle<int>{231, 18, 25, 25};
	const auto dozerImageRect = NAS2D::Rectangle<int>{206, 18, 25, 25};
	const auto diggerImageRect = NAS2D::Rectangle<int>{181, 18, 25, 25};
	const auto robotSummaryImageRect = NAS2D::Rectangle<int>{231, 43, 25, 25};

	struct IconPartsTotal {
		const NAS2D::Rectangle<int>& imageRect;
		int parts;
		std::size_t total;
	};

	const std::array<IconPartsTotal, 4> icons = {{
		{minerImageRect, mRobotPool.getAvailableCount(ROBOT_MINER), mRobotPool.miners().size()},
		{dozerImageRect, mRobotPool.getAvailableCount(ROBOT_DOZER), mRobotPool.dozers().size()},
		{diggerImageRect, mRobotPool.getAvailableCount(ROBOT_DIGGER), mRobotPool.diggers().size()},
		{robotSummaryImageRect, static_cast<int>(mRobotPool.currentControlCount()), mRobotPool.robotControlMax()},
	}};

	for (const auto params : icons) {
		renderer.drawSubImage(mUiIcons, position, params.imageRect);
		renderer.drawText(*MAIN_FONT, NAS2D::string_format("%i/%i", params.parts, params.total), position + textOffset, NAS2D::Color::White);
		position.y() -= 25;
	}
}

bool MapViewState::drawNavIcon(NAS2D::Renderer& renderer, const NAS2D::Rectangle_2d& currentIconBounds, const NAS2D::Rectangle_2d& subImageBounds, const NAS2D::Color& iconColor, const NAS2D::Color& iconHighlightColor) {
	bool isMouseInIcon = currentIconBounds.contains(MOUSE_COORDS);
	NAS2D::Color color = isMouseInIcon ? iconHighlightColor : iconColor;
	renderer.drawSubImage(mUiIcons, currentIconBounds.startPoint(), subImageBounds, color);
	return isMouseInIcon;
}

/**
 * Draws navigation UI.
 */
void MapViewState::drawNavInfo()
{
	NAS2D::Renderer& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	drawNavIcon(renderer, MOVE_DOWN_ICON, NAS2D::Rectangle_2d{64, 128, 32, 32}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, MOVE_UP_ICON, NAS2D::Rectangle_2d{96, 128, 32, 32}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, MOVE_EAST_ICON, NAS2D::Rectangle_2d{32, 128, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, MOVE_WEST_ICON, NAS2D::Rectangle_2d{32, 144, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, MOVE_NORTH_ICON, NAS2D::Rectangle_2d{0, 128, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);
	drawNavIcon(renderer, MOVE_SOUTH_ICON, NAS2D::Rectangle_2d{0, 144, 32, 16}, NAS2D::Color::White, NAS2D::Color::Red);

	// display the levels "bar"
	int iWidth = MAIN_FONT->width("IX"); // set steps character patern width
	int iPosX = static_cast<int>(renderer.width()) - 5; // set start position from right border
	int iPosY = mMiniMapBoundingBox.y() - MAIN_FONT->height() - 30; // set vertical position

	for (int i = mTileMap->maxDepth(); i >= 0; i--)
	{
		const auto levelString = (i == 0) ? std::string{"S"} : std::to_string(i); // Set string for current level
		bool isCurrentDepth = i == mTileMap->currentDepth();
		NAS2D::Color color = isCurrentDepth ? NAS2D::Color::Red : NAS2D::Color{200, 200, 200}; // red for current depth : white for others
		const auto position = NAS2D::Point<int>{iPosX - MAIN_FONT->width(levelString), iPosY}.to<float>();
		renderer.drawText(*MAIN_FONT, levelString, position, color);
		iPosX = iPosX - iWidth; // Shift position by one step left
	}
}
