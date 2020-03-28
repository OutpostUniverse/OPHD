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

extern Rectangle_2d MENU_ICON;

extern Rectangle_2d MOVE_NORTH_ICON;
extern Rectangle_2d MOVE_SOUTH_ICON;
extern Rectangle_2d MOVE_EAST_ICON;
extern Rectangle_2d MOVE_WEST_ICON;
extern Rectangle_2d MOVE_UP_ICON;
extern Rectangle_2d MOVE_DOWN_ICON;

extern Point_2d MOUSE_COORDS;

extern Font* MAIN_FONT; /// yuck
extern std::vector<void*> path;


namespace {
	uint8_t getGlowIntensity()
	{
		static Timer glowTimer;
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
		return glowStep;
	}
}


/**
 * Draws the minimap and all icons/overlays for it.
 */
void MapViewState::drawMiniMap()
{
	Renderer& renderer = Utility<Renderer>::get();
	const auto miniMapBoxFloat = mMiniMapBoundingBox.to<float>();
	renderer.clipRect(miniMapBoxFloat);

	bool isHeightmapToggled = mBtnToggleHeightmap.toggled();
	renderer.drawImage(isHeightmapToggled ? mHeightMap : mMapDisplay, miniMapBoxFloat.startPoint());

	if (ccLocationX() != 0 && ccLocationY() != 0)
	{
		renderer.drawSubImage(mUiIcons, ccLocationX() + mMiniMapBoundingBox.x() - 15, ccLocationY() + mMiniMapBoundingBox.y() - 15, 166, 226, 30, 30);
		renderer.drawBoxFilled(ccLocationX() + mMiniMapBoundingBox.x() - 1, ccLocationY() + mMiniMapBoundingBox.y() - 1, 3, 3, 255, 255, 255);
	}

	for (auto _tower : Utility<StructureManager>::get().structureList(Structure::CLASS_COMM))
	{
		if (_tower->operational())
		{
			Tile* t = Utility<StructureManager>::get().tileFromStructure(_tower);
			renderer.drawSubImage(mUiIcons, t->x() + mMiniMapBoundingBox.x() - 10, t->y() + mMiniMapBoundingBox.y() - 10, 146, 236, 20, 20);
		}
	}

	for (auto _mine : mTileMap->mineLocations())
	{
		Mine* mine = mTileMap->getTile(_mine, 0)->mine();
		if (!mine) { break; } // avoids potential race condition where a mine is destroyed during an updated cycle.

		float mineBeaconStatusOffsetX = 0.0f;
		if (!mine->active()) { mineBeaconStatusOffsetX = 0.0f; }
		else if (!mine->exhausted()) { mineBeaconStatusOffsetX = 8.0f; }
		else { mineBeaconStatusOffsetX = 16.0f; }

		renderer.drawSubImage(mUiIcons, _mine.x() + mMiniMapBoundingBox.x() - 2, _mine.y() + mMiniMapBoundingBox.y() - 2, mineBeaconStatusOffsetX, 0.0f, 7.0f, 7.0f);
	}

	for (auto _tile : path)
	{
		const auto tilePosition = static_cast<Tile*>(_tile)->position();
		renderer.drawPoint(tilePosition.x() + mMiniMapBoundingBox.x(), tilePosition.y() + mMiniMapBoundingBox.y(), NAS2D::Color::Magenta);
	}

	for (auto _robot : mRobotList)
	{
		renderer.drawPoint(_robot.second->x() + mMiniMapBoundingBox.x(), _robot.second->y() + mMiniMapBoundingBox.y(), NAS2D::Color::Cyan);
	}

	const auto& viewLocation = mTileMap->mapViewLocation();

	renderer.drawBox(mMiniMapBoundingBox.x() + viewLocation.x() + 1, mMiniMapBoundingBox.y() + viewLocation.y() + 1, mTileMap->edgeLength(), mTileMap->edgeLength(), 0, 0, 0, 180);
	renderer.drawBox(mMiniMapBoundingBox.x() + viewLocation.x(), mMiniMapBoundingBox.y() + viewLocation.y(), mTileMap->edgeLength(), mTileMap->edgeLength(), 255, 255, 255);

	renderer.clipRectClear();
}


/**
 * Draws the resource information bar.
 */
void MapViewState::drawResourceInfo()
{
	Renderer& renderer = Utility<Renderer>::get();

	renderer.drawBoxFilled(0, 0, renderer.width(), constants::RESOURCE_ICON_SIZE + 4, 39, 39, 39);
	renderer.drawBox(0, 0, renderer.width(), constants::RESOURCE_ICON_SIZE + 4, 21, 21, 21);
	renderer.drawLine(1, 0, renderer.width() - 2, 0, 56, 56, 56);

	// Resources
	int x = constants::MARGIN_TIGHT + 12;
	int offsetX = constants::RESOURCE_ICON_SIZE + 40;
	auto position = NAS2D::Point<int>{constants::MARGIN_TIGHT + 12, constants::MARGIN_TIGHT};
	constexpr auto textOffset = NAS2D::Vector<int>{constants::RESOURCE_ICON_SIZE + constants::MARGIN, 3 - constants::MARGIN_TIGHT};

	renderer.drawSubImage(mUiIcons, 2, 7, mPinResourcePanel ? 8 : 0, 72, 8, 8);
	renderer.drawSubImage(mUiIcons, 675, 7, mPinPopulationPanel ? 8 : 0, 72, 8, 8);

	const auto glowStep = getGlowIntensity();

	const auto glowColor = NAS2D::Color{255, glowStep, glowStep};

	// Common Metals
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 64, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	bool shouldCommonMetalsGlow = mPlayerResources.commonMetals() <= 10;
	auto color = shouldCommonMetalsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.commonMetals()), position + textOffset, color);

	// Rare Metals
	position.x() += offsetX;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 80, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	bool shouldRareMetalsGlow = mPlayerResources.rareMetals() <= 10;
	color = shouldRareMetalsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.rareMetals()), position + textOffset, color);

	// Common Minerals
	position.x() += x + offsetX;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 96, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	bool shouldCommonMineralsGlow = mPlayerResources.commonMinerals() <= 10;
	color = shouldCommonMineralsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.commonMinerals()), position + textOffset, color);

	// Rare Minerals
	position.x() += x + offsetX;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 112, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	bool shouldRareMineralsGlow = mPlayerResources.rareMinerals() <= 10;
	color = shouldRareMineralsGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, std::to_string(mPlayerResources.rareMinerals()), position + textOffset, color);

	// Storage Capacity
	position.x() += x + offsetX;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 96, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	bool shouldStorageCapacityGlow = mPlayerResources.capacity() - mPlayerResources.currentLevel() <= 100;
	color = shouldStorageCapacityGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.currentLevel(), mPlayerResources.capacity()), position + textOffset, color);

	// Food
	position.x() += (x + offsetX) * 2;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 64, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	bool shouldFoodStorageGlow = foodInStorage() <= 10;
	color = shouldFoodStorageGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, string_format("%i/%i", foodInStorage(), foodTotalStorage()), position + textOffset, color);

	// Energy
	position.x() += (x + offsetX) * 2;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 80, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	bool shouldEnergyGlow = mPlayerResources.energy() <= 5;
	color = shouldEnergyGlow ? glowColor : NAS2D::Color::White;
	renderer.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.energy(), Utility<StructureManager>::get().totalEnergyProduction()), position + textOffset, color);

	// Population / Morale
	position.x() += (x + offsetX) * 2;
	int popMoraleDeltaImageOffsetX = mCurrentMorale < mPreviousMorale ? 0 : (mCurrentMorale > mPreviousMorale ? 16 : 32);
	renderer.drawSubImage(mUiIcons, position.x() - 17, position.y(), popMoraleDeltaImageOffsetX, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);

	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 176 + (std::clamp(mCurrentMorale, 1, 999) / 200) * constants::RESOURCE_ICON_SIZE, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	renderer.drawText(*MAIN_FONT, std::to_string(mPopulation.size()), position + textOffset, NAS2D::Color::White);

	bool isMouseInPopPanel = NAS2D::Rectangle{675, 1, 75, 19}.contains(MOUSE_COORDS);
	bool shouldShowPopPanel = mPinPopulationPanel || isMouseInPopPanel;
	if(shouldShowPopPanel) { mPopulationPanel.update(); }

	bool isMouseInResourcePanel = NAS2D::Rectangle{0, 1, static_cast<int>(mResourceBreakdownPanel.width()), 19}.contains(MOUSE_COORDS);
	bool shouldShowResourcePanel = mPinResourcePanel || isMouseInResourcePanel;
	if (shouldShowResourcePanel) { mResourceBreakdownPanel.update(); }

	// Turns
	renderer.drawSubImage(mUiIcons, renderer.width() - 80, position.y(), 128, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	renderer.drawText(*MAIN_FONT, std::to_string(mTurnCount), renderer.width() - 80 + textOffset.x, position.y() + textOffset.y, 255, 255, 255);

	bool isMouseInMenu = MENU_ICON.contains(MOUSE_COORDS);
	int menuGearHighlightOffsetX = isMouseInMenu ? 144 : 128;
	renderer.drawSubImage(mUiIcons, MENU_ICON.x() + constants::MARGIN_TIGHT, MENU_ICON.y() + constants::MARGIN_TIGHT, menuGearHighlightOffsetX, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);

}


/**
 * Draws robot deployment information.
 */
void MapViewState::drawRobotInfo()
{
	// CC hasn't been placed yet.
	if (ccLocationX() == 0) { return; }

	Renderer& renderer = Utility<Renderer>::get();

	// Robots
	// Start from the bottom - The bottom UI Height - Icons Height - 8 (1 offset to avoid the last to be glued with at the border)
	auto position = NAS2D::Point<int>{8, static_cast<int>(renderer.height()) - constants::BOTTOM_UI_HEIGHT - 25 - 8};
	constexpr auto textOffset = NAS2D::Vector<int>{30, 7};

	// Miner (last one)
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 231, 18, 25, 25);
	renderer.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_MINER), mRobotPool.miners().size()), position + textOffset, NAS2D::Color::White);
	// Dozer (Midle one)
	position.y() -= 25;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 206, 18, 25, 25);
	renderer.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DOZER), mRobotPool.dozers().size()), position + textOffset, NAS2D::Color::White);
	// Digger (First one)
	position.y() -= 25;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 181, 18, 25, 25);
	renderer.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DIGGER), mRobotPool.diggers().size()), position + textOffset, NAS2D::Color::White);
	// robot control summary
	position.y() -= 25;
	renderer.drawSubImage(mUiIcons, position.x(), position.y(), 231, 43, 25, 25);
	renderer.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.currentControlCount(), mRobotPool.robotControlMax()), position + textOffset, NAS2D::Color::White);
}

bool MapViewState::drawNavIcon(Renderer& renderer, const NAS2D::Rectangle_2d& currentIconBounds, const NAS2D::Rectangle_2d& subImageBounds, const NAS2D::Color& iconColor, const NAS2D::Color& iconHighlightColor) {
	bool isMouseInIcon = currentIconBounds.contains(MOUSE_COORDS);
	NAS2D::Color color = isMouseInIcon ? iconHighlightColor : iconColor;
	renderer.drawSubImage(mUiIcons, currentIconBounds.x(), currentIconBounds.y(), subImageBounds.x(), subImageBounds.y(), subImageBounds.width(), subImageBounds.height(), color.red(), color.green(), color.blue(), color.alpha());
	return isMouseInIcon;
}

/**
 * Draws navigation UI.
 */
void MapViewState::drawNavInfo()
{
	Renderer& renderer = Utility<Renderer>::get();

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
		NAS2D::Color color = isCurrentDepth ? NAS2D::Color{255, 0, 0, 255} : NAS2D::Color{200, 200, 200, 255}; // red for current depth : white for others
		const auto position = NAS2D::Point<int>{iPosX - MAIN_FONT->width(levelString), iPosY}.to<float>();
		renderer.drawText(*MAIN_FONT, levelString, position.x(), position.y(), color.red(), color.green(), color.blue(), color.alpha());
		iPosX = iPosX - iWidth; // Shift position by one step left
	}
}
