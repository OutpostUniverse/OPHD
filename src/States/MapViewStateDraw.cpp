// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"

#include "../Constants.h"
#include "../FontManager.h"

extern Rectangle_2d MENU_ICON;

extern Rectangle_2d MOVE_NORTH_ICON;
extern Rectangle_2d MOVE_SOUTH_ICON;
extern Rectangle_2d MOVE_EAST_ICON;
extern Rectangle_2d MOVE_WEST_ICON;
extern Rectangle_2d MOVE_UP_ICON;
extern Rectangle_2d MOVE_DOWN_ICON;

extern Point_2d MOUSE_COORDS;


std::string S_LEVEL;


Timer GLOW_TIMER;
const int GLOW_STEP_SIZE = 20;

int GLOW_STEP;
int GLOW_STEP_DIRECTION = 1;


extern Font* MAIN_FONT; /// yuck


static void updateGlowTimer()
{
	if (GLOW_TIMER.accumulator() >= 10)
	{
		GLOW_STEP += GLOW_STEP_SIZE * GLOW_STEP_DIRECTION;
		GLOW_TIMER.reset();
	}

	if (GLOW_STEP >= 255)
	{
		GLOW_STEP = 255;
		GLOW_STEP_DIRECTION = -1;
	}
	else if (GLOW_STEP <= 0)
	{
		GLOW_STEP = 0;
		GLOW_STEP_DIRECTION = 1;
	}
}


/**
 * Draws the minimap and all icons/overlays for it.
 */
void MapViewState::drawMiniMap()
{
	Renderer& r = Utility<Renderer>::get();
	r.clipRect(mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y(), mMiniMapBoundingBox.width(), mMiniMapBoundingBox.height());

	if (mBtnToggleHeightmap.toggled()) { r.drawImage(mHeightMap, mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y()); }
	else { r.drawImage(mMapDisplay, mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y()); }

	if (ccLocationX() != 0 && ccLocationY() != 0)
	{
		r.drawSubImage(mUiIcons, ccLocationX() + mMiniMapBoundingBox.x() - 15, ccLocationY() + mMiniMapBoundingBox.y() - 15, 166, 226, 30, 30);
		r.drawBoxFilled(ccLocationX() + mMiniMapBoundingBox.x() - 1, ccLocationY() + mMiniMapBoundingBox.y() - 1, 3, 3, 255, 255, 255);
	}

	for (auto _tower : Utility<StructureManager>::get().structureList(Structure::CLASS_COMM))
	{
		if (_tower->operational())
		{
			Tile* t = Utility<StructureManager>::get().tileFromStructure(_tower);
			r.drawSubImage(mUiIcons, t->x() + mMiniMapBoundingBox.x() - 10, t->y() + mMiniMapBoundingBox.y() - 10, 146, 236, 20, 20);
		}
	}

	for (auto _mine : mTileMap->mineLocations())
	{
		Mine* mine = mTileMap->getTile(_mine.x(), _mine.y(), 0)->mine();
		if (!mine) { break; } // avoids potential race condition where a mine is destroyed during an updated cycle.

		if (!mine->active())
		{
			r.drawSubImage(mUiIcons, _mine.x() + mMiniMapBoundingBox.x() - 2, _mine.y() + mMiniMapBoundingBox.y() - 2, 0.0f, 0.0f, 7.0f, 7.0f);
		}
		else if (mine->active() && !mine->exhausted())
		{
			r.drawSubImage(mUiIcons, _mine.x() + mMiniMapBoundingBox.x() - 2, _mine.y() + mMiniMapBoundingBox.y() - 2, 8.0f, 0.0f, 7.0f, 7.0f);
		}
		else if (mine->exhausted())
		{
			r.drawSubImage(mUiIcons, _mine.x() + mMiniMapBoundingBox.x() - 2, _mine.y() + mMiniMapBoundingBox.y() - 2, 16.0f, 0.0f, 7.0f, 7.0f);
		}

	}

	for (auto _robot : mRobotList)
	{
		r.drawPoint(_robot.second->x() + mMiniMapBoundingBox.x(), _robot.second->y() + mMiniMapBoundingBox.y(), 0, 255, 255);
	}

	const Point_2d& _pt = mTileMap->mapViewLocation();

	r.drawBox(mMiniMapBoundingBox.x() + _pt.x() + 1, mMiniMapBoundingBox.y() + _pt.y() + 1, mTileMap->edgeLength(), mTileMap->edgeLength(), 0, 0, 0, 180);
	r.drawBox(mMiniMapBoundingBox.x() + _pt.x(), mMiniMapBoundingBox.y() + _pt.y(), mTileMap->edgeLength(), mTileMap->edgeLength(), 255, 255, 255);

	r.clipRectClear();
}


/**
 * Draws the resource information bar.
 */
void MapViewState::drawResourceInfo()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(0, 0, r.width(), constants::RESOURCE_ICON_SIZE + 4, 39, 39, 39);
	r.drawBox(0, 0, r.width(), constants::RESOURCE_ICON_SIZE + 4, 21, 21, 21);
	r.drawLine(1, 0, r.width() - 2, 0, 56, 56, 56);

	// Resources
	int x = constants::MARGIN_TIGHT + 12;
	int y = constants::MARGIN_TIGHT;

	int textY = 3;
	int offsetX = constants::RESOURCE_ICON_SIZE + 40;
	int margin = constants::RESOURCE_ICON_SIZE + constants::MARGIN;

	r.drawSubImage(mUiIcons, 2, 7, mPinResourcePanel ? 8 : 0, 72, 8, 8);
	r.drawSubImage(mUiIcons, 675, 7, mPinPopulationPanel ? 8 : 0, 72, 8, 8);

	updateGlowTimer();

	// Common Metals
	r.drawSubImage(mUiIcons, x, y , 64, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.commonMetals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMetals()), x + margin, textY, 255, GLOW_STEP, GLOW_STEP); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMetals()), x + margin, textY, 255, 255, 255); }

	// Rare Metals
	r.drawSubImage(mUiIcons, x + offsetX, y, 80, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.rareMetals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMetals()), (x + offsetX) + margin, textY, 255, GLOW_STEP, GLOW_STEP); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMetals()), (x + offsetX) + margin, textY, 255, 255, 255); }

	// Common Minerals
	r.drawSubImage(mUiIcons, (x + offsetX) * 2, y, 96, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.commonMinerals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMinerals()), (x + offsetX) * 2 + margin, textY, 255, GLOW_STEP, GLOW_STEP); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMinerals()), (x + offsetX) * 2 + margin, textY, 255, 255, 255); }

	// Rare Minerals
	r.drawSubImage(mUiIcons, (x + offsetX) * 3, y, 112, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.rareMinerals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMinerals()), (x + offsetX) * 3 + margin, textY, 255, GLOW_STEP, GLOW_STEP); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMinerals()), (x + offsetX) * 3 + margin, textY, 255, 255, 255); }

	// Storage Capacity
	r.drawSubImage(mUiIcons, (x + offsetX) * 4, y, 96, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.capacity() - mPlayerResources.currentLevel() <= 100) { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.currentLevel(), mPlayerResources.capacity()), (x + offsetX) * 4 + margin, textY, 255, GLOW_STEP, GLOW_STEP); }
	else { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.currentLevel(), mPlayerResources.capacity()), (x + offsetX) * 4 + margin, textY, 255, 255, 255); }

	// Food
	r.drawSubImage(mUiIcons, (x + offsetX) * 6, y, 64, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (foodInStorage() <= 10) { r.drawText(*MAIN_FONT, string_format("%i/%i", foodInStorage(), foodTotalStorage()), (x + offsetX) * 6 + margin, textY, 255, GLOW_STEP, GLOW_STEP); }
	else { r.drawText(*MAIN_FONT, string_format("%i/%i", foodInStorage(), foodTotalStorage()), (x + offsetX) * 6 + margin, textY, 255, 255, 255); }

	// Energy
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 80, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.energy() <= 5) { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.energy(), Utility<StructureManager>::get().totalEnergyProduction()), (x + offsetX) * 8 + margin, textY, 255, GLOW_STEP, GLOW_STEP); }
	else { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.energy(), Utility<StructureManager>::get().totalEnergyProduction()), (x + offsetX) * 8 + margin, textY, 255, 255, 255); }

	// Population / Morale
	if (mCurrentMorale > mPreviousMorale) { r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 16, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
	else if (mCurrentMorale < mPreviousMorale) { r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 0, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
	else { r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 32, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }

	r.drawSubImage(mUiIcons, (x + offsetX) * 10, y, 176 + (clamp(mCurrentMorale, 1, 999) / 200) * constants::RESOURCE_ICON_SIZE, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(*MAIN_FONT, string_format("%i", mPopulation.size()), (x + offsetX) * 10 + margin, textY, 255, 255, 255);

	if (mPinPopulationPanel	|| isPointInRect(MOUSE_COORDS.x(), MOUSE_COORDS.y(), 675, 1, 75, 19)) { mPopulationPanel.update(); }
	if (mPinResourcePanel	|| isPointInRect(MOUSE_COORDS.x(), MOUSE_COORDS.y(), 0, 1, mResourceBreakdownPanel.width(), 19)) { mResourceBreakdownPanel.update(); }

	// Turns
	r.drawSubImage(mUiIcons, r.width() - 80, y, 128, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(*MAIN_FONT, string_format("%i", mTurnCount), r.width() - 80 + margin, textY, 255, 255, 255);

	if (isPointInRect(MOUSE_COORDS, MENU_ICON)) { r.drawSubImage(mUiIcons, MENU_ICON.x() + constants::MARGIN_TIGHT, MENU_ICON.y() + constants::MARGIN_TIGHT, 144, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
	else { r.drawSubImage(mUiIcons, MENU_ICON.x() + constants::MARGIN_TIGHT, MENU_ICON.y() + constants::MARGIN_TIGHT, 128, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
}


/**
 * Draws robot deployment information.
 */
void MapViewState::drawRobotInfo()
{
	// CC hasn't been placed yet.
	if (ccLocationX() == 0) { return; }
	
	Renderer& r = Utility<Renderer>::get();

	// Robots
	// Start from the bottom - The bottom UI Height - Icons Height - 8 (1 offset to avoid the last to be glued with at the border)
	int y = (int)r.height() - constants::BOTTOM_UI_HEIGHT - 25 - 8;
	int textY = y + 7;	// Same position + 10 to center the text with the graphics
	int margin = 30;	// Margin of 28 px from the graphics to the text
	int x = 0, offsetX = 1;	// Start a the left side of the screen + an offset of 1 to detatch from the border
	
	// Miner (last one)
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 231, 18, 25, 25);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_MINER), mRobotPool.miners().size()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
	// Dozer (Midle one)
	textY -= 25; y -= 25;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 206, 18, 25, 25);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DOZER), mRobotPool.dozers().size()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
	// Digger (First one)
	textY -= 25; y -= 25;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 181, 18, 25, 25);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DIGGER), mRobotPool.diggers().size()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
	// robot control summary
	textY -= 25; y -= 25;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 231, 43, 25, 25);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.currentControlCount(), mRobotPool.robotControlMax()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
}


/**
 * Draws navigation UI.
 */
void MapViewState::drawNavInfo()
{
	Renderer& r = Utility<Renderer>::get();

	// Up / Down
	if (isPointInRect(MOUSE_COORDS, MOVE_DOWN_ICON))
	{
		r.drawSubImage(mUiIcons, MOVE_DOWN_ICON.x(), MOVE_DOWN_ICON.y(), 64, 128, 32, 32, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, MOVE_DOWN_ICON.x(), MOVE_DOWN_ICON.y(), 64, 128, 32, 32);
	}

	if (isPointInRect(MOUSE_COORDS, MOVE_UP_ICON))
	{
		r.drawSubImage(mUiIcons, MOVE_UP_ICON.x(), MOVE_UP_ICON.y(), 96, 128, 32, 32, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, MOVE_UP_ICON.x(), MOVE_UP_ICON.y(), 96, 128, 32, 32);
	}

	// East / West / North / South
	if (isPointInRect(MOUSE_COORDS, MOVE_EAST_ICON))
	{
		r.drawSubImage(mUiIcons, MOVE_EAST_ICON.x(), MOVE_EAST_ICON.y(), 32, 128, 32, 16, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, MOVE_EAST_ICON.x(), MOVE_EAST_ICON.y(), 32, 128, 32, 16);
	}

	if (isPointInRect(MOUSE_COORDS, MOVE_WEST_ICON))
	{
		r.drawSubImage(mUiIcons, MOVE_WEST_ICON.x(), MOVE_WEST_ICON.y(), 32, 144, 32, 16, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, MOVE_WEST_ICON.x(), MOVE_WEST_ICON.y(), 32, 144, 32, 16);
	}

	if (isPointInRect(MOUSE_COORDS, MOVE_NORTH_ICON))
	{
		r.drawSubImage(mUiIcons, MOVE_NORTH_ICON.x(), MOVE_NORTH_ICON.y(), 0, 128, 32, 16, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, MOVE_NORTH_ICON.x(), MOVE_NORTH_ICON.y(), 0, 128, 32, 16);
	}

	if (isPointInRect(MOUSE_COORDS, MOVE_SOUTH_ICON))
	{
		r.drawSubImage(mUiIcons, MOVE_SOUTH_ICON.x(), MOVE_SOUTH_ICON.y(), 0, 144, 32, 16, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, MOVE_SOUTH_ICON.x(), MOVE_SOUTH_ICON.y(), 0, 144, 32, 16);
	}


	// display the levels "bar"
	int iWidth = MAIN_FONT->width("IX");								// set steps character patern width
	int iPosX = r.width() - 5;											// set start position from right border
	int iPosY = mMiniMapBoundingBox.y() - MAIN_FONT->height() - 30;	// set vertical position
	
	for (int i = mTileMap->maxDepth(); i >= 0; i--)
	{
		S_LEVEL = string_format("%i", i);	// Set string for current level
		if (i == 0) { S_LEVEL = "S"; }		// surface level
		if (i == mTileMap->currentDepth())
		{
			r.drawText(*MAIN_FONT, S_LEVEL, iPosX - MAIN_FONT->width(S_LEVEL), iPosY, 255, 0, 0);		// Others in white
		}
		else
		{
			r.drawText(*MAIN_FONT, S_LEVEL, iPosX - MAIN_FONT->width(S_LEVEL), iPosY, 200, 200, 200);	// current one in red
		}

		iPosX = iPosX - iWidth;				// Shift position by one step left
	}
}


/**
 * Displays debug information.
 */
void MapViewState::drawDebug()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawText(*MAIN_FONT, string_format("FPS: %i", mFps.fps()), 10, 25, 255, 255, 255);
	r.drawText(*MAIN_FONT, string_format("Map Dimensions: %i, %i", mTileMap->width(), mTileMap->height()), 10, 25 + MAIN_FONT->height(), 255, 255, 255);
	r.drawText(*MAIN_FONT, string_format("Max Digging Depth: %i", mTileMap->maxDepth()), 10, 25 + MAIN_FONT->height() * 2, 255, 255, 255);
	r.drawText(*MAIN_FONT, string_format("Map Mouse Hover Coords: %i, %i", mTileMap->tileMouseHoverX(), mTileMap->tileMouseHoverY()), 10, 25 + MAIN_FONT->height() * 3, 255, 255, 255);
	r.drawText(*MAIN_FONT, string_format("Current Depth: %i", mTileMap->currentDepth()), 10, 25 + MAIN_FONT->height() * 4, 255, 255, 255);

	r.drawText(*MAIN_FONT, string_format("Structure Count: %i", Utility<StructureManager>::get().count()), 10, 25 + MAIN_FONT->height() * 6, 255, 255, 255);
}
