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

extern std::vector<void*> path;

/**
 * Draws the minimap and all icons/overlays for it.
 */
void MapViewState::drawMiniMap()
{
	Renderer& r = Utility<Renderer>::get();
	auto miniMapX = static_cast<float>(mMiniMapBoundingBox.x());
	auto miniMapY = static_cast<float>(mMiniMapBoundingBox.y());
	float clipX = static_cast<float>(miniMapX);
	float clipY = static_cast<float>(miniMapY);
	float clipW = static_cast<float>(mMiniMapBoundingBox.width());
	float clipH = static_cast<float>(mMiniMapBoundingBox.height());
	r.clipRect(clipX, clipY, clipW, clipH);

	if (mBtnToggleHeightmap.toggled()) { r.drawImage(mHeightMap, clipX, clipY); }
	else { r.drawImage(mMapDisplay, clipX, clipY); }

	if (ccLocationX() != 0 && ccLocationY() != 0)
	{
		r.drawSubImage(mUiIcons, ccLocationX() + miniMapX - 15.0f, ccLocationY() + miniMapY - 15.0f, 166, 226, 30, 30);
		r.drawBoxFilled(ccLocationX() + miniMapX - 1.0f, ccLocationY() + miniMapY - 1.0f, 3.0f, 3.0f, 255, 255, 255);
	}

	for (auto _tower : Utility<StructureManager>::get().structureList(Structure::CLASS_COMM))
	{
		if (_tower->operational())
		{
			Tile* t = Utility<StructureManager>::get().tileFromStructure(_tower);
			r.drawSubImage(mUiIcons, t->x() + miniMapX - 10.0f, t->y() + miniMapY - 10.0f, 146.0f, 236.0f, 20.0f, 20.0f);
		}
	}

	for (auto mine_tile_coords : mTileMap->mineLocations())
	{
		auto mine_tcX = mine_tile_coords.x();
		auto mine_tcY = mine_tile_coords.y();
		Mine* mine = mTileMap->getTile(mine_tcX, mine_tcY, 0)->mine();
		if (!mine) { break; } // avoids potential race condition where a mine is destroyed during an updated cycle.

		if (!mine->active())
		{
			r.drawSubImage(mUiIcons, mine_tcX + miniMapX - 2.0f, mine_tcY + miniMapY - 2, 0.0f, 0.0f, 7.0f, 7.0f);
		}
		else if (mine->active() && !mine->exhausted())
		{
			r.drawSubImage(mUiIcons, mine_tcX + miniMapX - 2.0f, mine_tcY + miniMapY - 2.0f, 8.0f, 0.0f, 7.0f, 7.0f);
		}
		else if (mine->exhausted())
		{
			r.drawSubImage(mUiIcons, mine_tcX + miniMapX - 2.0f, mine_tcY + miniMapY - 2.0f, 16.0f, 0.0f, 7.0f, 7.0f);
		}

	}

	for (auto _tile : path)
	{
		r.drawPoint(static_cast<Tile*>(_tile)->x() + miniMapX, static_cast<Tile*>(_tile)->y() + miniMapY, NAS2D::Color::Magenta);
	}

	for (auto _robot : mRobotList)
	{
		r.drawPoint(_robot.second->x() + miniMapX, _robot.second->y() + miniMapY, 0, 255, 255);
	}

	const Point_2d& _pt = mTileMap->mapViewLocation();
	float edge_length = static_cast<float>(mTileMap->edgeLength());
	r.drawBox(miniMapX + _pt.x() + 1, miniMapY + _pt.y() + 1, edge_length, edge_length, 0, 0, 0, 180);
	r.drawBox(miniMapX + _pt.x(), miniMapY + _pt.y(), edge_length, edge_length, 255, 255, 255);

	r.clipRectClear();
}


/**
 * Draws the resource information bar.
 */
void MapViewState::drawResourceInfo()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(0.0f, 0.0f, r.width(), constants::RESOURCE_ICON_SIZE + 4, 39, 39, 39);
	r.drawBox(0.0f, 0.0f, r.width(), constants::RESOURCE_ICON_SIZE + 4.0f, 21, 21, 21);
	r.drawLine(1.0f, 0.0f, r.width() - 2.0f, 0.0f, 56, 56, 56);

	// Resources
	float x = constants::MARGIN_TIGHT + 12.0f;
	float y = constants::MARGIN_TIGHT;

	float textY = 3.0f;
	float offsetX = constants::RESOURCE_ICON_SIZE + 40.0f;
	float margin = constants::RESOURCE_ICON_SIZE + constants::MARGIN;

	r.drawSubImage(mUiIcons, 2.0f, 7.0f, mPinResourcePanel ? 8.0f : 0.0f, 72.0f, 8.0f, 8.0f);
	r.drawSubImage(mUiIcons, 675.0f, 7.0f, mPinPopulationPanel ? 8.0f : 0.0f, 72.0f, 8.0f, 8.0f);

	updateGlowTimer();

	// Common Metals
	r.drawSubImage(mUiIcons, x, y , 64, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	auto glowStepAsUint8 = static_cast<uint8_t>(GLOW_STEP);
	if (mPlayerResources.commonMetals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMetals()), x + margin, textY, 255, glowStepAsUint8, glowStepAsUint8); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMetals()), x + margin, textY, 255, 255, 255); }

	// Rare Metals
	r.drawSubImage(mUiIcons, x + offsetX, y, 80, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.rareMetals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMetals()), (x + offsetX) + margin, textY, 255, glowStepAsUint8, glowStepAsUint8); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMetals()), (x + offsetX) + margin, textY, 255, 255, 255); }

	// Common Minerals
	r.drawSubImage(mUiIcons, (x + offsetX) * 2, y, 96, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.commonMinerals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMinerals()), (x + offsetX) * 2 + margin, textY, 255, glowStepAsUint8, glowStepAsUint8); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.commonMinerals()), (x + offsetX) * 2 + margin, textY, 255, 255, 255); }

	// Rare Minerals
	r.drawSubImage(mUiIcons, (x + offsetX) * 3, y, 112, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.rareMinerals() <= 10) { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMinerals()), (x + offsetX) * 3 + margin, textY, 255, glowStepAsUint8, glowStepAsUint8); }
	else { r.drawText(*MAIN_FONT, string_format("%i", mPlayerResources.rareMinerals()), (x + offsetX) * 3 + margin, textY, 255, 255, 255); }

	// Storage Capacity
	r.drawSubImage(mUiIcons, (x + offsetX) * 4, y, 96, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.capacity() - mPlayerResources.currentLevel() <= 100) { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.currentLevel(), mPlayerResources.capacity()), (x + offsetX) * 4 + margin, textY, 255, glowStepAsUint8, glowStepAsUint8); }
	else { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.currentLevel(), mPlayerResources.capacity()), (x + offsetX) * 4 + margin, textY, 255, 255, 255); }

	// Food
	r.drawSubImage(mUiIcons, (x + offsetX) * 6, y, 64, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (foodInStorage() <= 10) { r.drawText(*MAIN_FONT, string_format("%i/%i", foodInStorage(), foodTotalStorage()), (x + offsetX) * 6 + margin, textY, 255, glowStepAsUint8, glowStepAsUint8); }
	else { r.drawText(*MAIN_FONT, string_format("%i/%i", foodInStorage(), foodTotalStorage()), (x + offsetX) * 6 + margin, textY, 255, 255, 255); }

	// Energy
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 80, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	if (mPlayerResources.energy() <= 5) { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.energy(), Utility<StructureManager>::get().totalEnergyProduction()), (x + offsetX) * 8 + margin, textY, 255, glowStepAsUint8, glowStepAsUint8); }
	else { r.drawText(*MAIN_FONT, string_format("%i/%i", mPlayerResources.energy(), Utility<StructureManager>::get().totalEnergyProduction()), (x + offsetX) * 8 + margin, textY, 255, 255, 255); }

	// Population / Morale
	if (mCurrentMorale > mPreviousMorale) { r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 16, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
	else if (mCurrentMorale < mPreviousMorale) { r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 0, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
	else { r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 32, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }

	r.drawSubImage(mUiIcons, (x + offsetX) * 10.0f, y, 176.0f + (std::clamp(mCurrentMorale, 1, 999) / 200) * constants::RESOURCE_ICON_SIZE, 0.0f, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(*MAIN_FONT, string_format("%i", mPopulation.size()), (x + offsetX) * 10 + margin, textY, 255, 255, 255);

	if (mPinPopulationPanel	|| isPointInRect(MOUSE_COORDS.x(), MOUSE_COORDS.y(), 675, 1, 75, 19)) { mPopulationPanel.update(); }
	if (mPinResourcePanel	|| isPointInRect(MOUSE_COORDS.x(), MOUSE_COORDS.y(), 0, 1, static_cast<int>(mResourceBreakdownPanel.width()), 19)) { mResourceBreakdownPanel.update(); }

	// Turns
	r.drawSubImage(mUiIcons, r.width() - 80, y, 128, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(*MAIN_FONT, string_format("%i", mTurnCount), r.width() - 80 + margin, textY, 255, 255, 255);

	if (isPointInRect(MOUSE_COORDS, MENU_ICON)) { r.drawSubImage(mUiIcons, static_cast<float>(MENU_ICON.x()) + constants::MARGIN_TIGHT, static_cast<float>(MENU_ICON.y()) + constants::MARGIN_TIGHT, 144.0f, 32.0f, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
	else { r.drawSubImage(mUiIcons, static_cast<float>(MENU_ICON.x()) + constants::MARGIN_TIGHT, static_cast<float>(MENU_ICON.y()) + constants::MARGIN_TIGHT, 128.0f, 32.0f, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE); }
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
	float y = r.height() - constants::BOTTOM_UI_HEIGHT - 25.0f - 8.0f;
	float textY = y + 7.0f;	// Same position + 10 to center the text with the graphics
	float margin = 30.0f;	// Margin of 28 px from the graphics to the text
	float x = 0.0f;
	float offsetX = 1.0f;	// Start a the left side of the screen + an offset of 1 to detatch from the border
	
	// Miner (last one)
	r.drawSubImage(mUiIcons, (x + offsetX) * 8.0f, y, 231.0f, 18.0f, 25.0f, 25.0f);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_MINER), mRobotPool.miners().size()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
	// Dozer (Midle one)
	textY -= 25; y -= 25;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8.0f, y, 206.0f, 18.0f, 25.0f, 25.0f);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DOZER), mRobotPool.dozers().size()), (x + offsetX) * 8.0f + margin, textY, 255, 255, 255);
	// Digger (First one)
	textY -= 25.0f;
	y -= 25.0f;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8.0f, y, 181.0f, 18.0f, 25.0f, 25.0f);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DIGGER), mRobotPool.diggers().size()), (x + offsetX) * 8.0f + margin, textY, 255, 255, 255);
	// robot control summary
	textY -= 25.0f;
	y -= 25.0f;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8.0f, y, 231.0f, 43.0f, 25.0f, 25.0f);
	r.drawText(*MAIN_FONT, string_format("%i/%i", mRobotPool.currentControlCount(), mRobotPool.robotControlMax()), (x + offsetX) * 8.0f + margin, textY, 255, 255, 255);
}


/**
 * Draws navigation UI.
 */
void MapViewState::drawNavInfo()
{
	Renderer& r = Utility<Renderer>::get();

	float move_icon_x = static_cast<float>(MOVE_DOWN_ICON.x());
	float move_icon_y = static_cast<float>(MOVE_DOWN_ICON.y());
	// Up / Down
	if (isPointInRect(MOUSE_COORDS, MOVE_DOWN_ICON))
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 64.0f, 128.0f, 32.0f, 32.0f, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 64.0f, 128.0f, 32.0f, 32.0f);
	}

    move_icon_x = static_cast<float>(MOVE_UP_ICON.x());
    move_icon_y = static_cast<float>(MOVE_UP_ICON.y());
	if (isPointInRect(MOUSE_COORDS, MOVE_UP_ICON))
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 96.0f, 128.0f, 32.0f, 32.0f, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 96.0f, 128.0f, 32.0f, 32.0f);
	}

    move_icon_x = static_cast<float>(MOVE_EAST_ICON.x());
    move_icon_y = static_cast<float>(MOVE_EAST_ICON.y());
	// East / West / North / South
	if (isPointInRect(MOUSE_COORDS, MOVE_EAST_ICON))
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 32.0f, 128.0f, 32.0f, 16.0f, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 32.0f, 128.0f, 32.0f, 16.0f);
	}

    move_icon_x = static_cast<float>(MOVE_WEST_ICON.x());
    move_icon_y = static_cast<float>(MOVE_WEST_ICON.y());
	if (isPointInRect(MOUSE_COORDS, MOVE_WEST_ICON))
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 32.0f, 144.0f, 32.0f, 16.0f, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 32.0f, 144.0f, 32.0f, 16.0f);
	}

    move_icon_x = static_cast<float>(MOVE_NORTH_ICON.x());
    move_icon_y = static_cast<float>(MOVE_NORTH_ICON.y());
	if (isPointInRect(MOUSE_COORDS, MOVE_NORTH_ICON))
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 0.0f, 128.0f, 32.0f, 16.0f, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 0.0f, 128.0f, 32.0f, 16.0f);
	}

    move_icon_x = static_cast<float>(MOVE_SOUTH_ICON.x());
    move_icon_y = static_cast<float>(MOVE_SOUTH_ICON.y());
	if (isPointInRect(MOUSE_COORDS, MOVE_SOUTH_ICON))
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 0.0f, 144.0f, 32.0f, 16.0f, 255, 0, 0, 255);
	}
	else
	{
		r.drawSubImage(mUiIcons, move_icon_x, move_icon_y, 0.0f, 144.0f, 32.0f, 16.0f);
	}


	// display the levels "bar"
	int iWidth = MAIN_FONT->width("IX");								// set steps character patern width
	float iPosX = r.width() - 5.0f;											// set start position from right border
	float iPosY = mMiniMapBoundingBox.y() - MAIN_FONT->height() - 30.0f;	// set vertical position
	
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
