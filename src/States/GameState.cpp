#include "GameState.h"
#include "PlanetSelectState.h"

#include "../Constants.h"
#include "../GraphWalker.h"
#include "../StructureCatalogue.h"
#include "../StructureTranslator.h"

#include "../Map/Tile.h"

#include "../Things/Robots/Robots.h"
#include "../Things/Structures/Structures.h"

#include <sstream>
#include <vector>

// Disable some warnings that can be safely ignored.
#pragma warning(disable: 4244) // possible loss of data (floats to int and vice versa)


using namespace std;
using namespace NAS2D;
using namespace NAS2D::Xml;


const std::string	MAP_TERRAIN_EXTENSION		= "_a.png";
const std::string	MAP_DISPLAY_EXTENSION		= "_b.png";

const int MAX_TILESET_INDEX	= 4;


Rectangle_2d MENU_ICON;

Rectangle_2d MOVE_NORTH_ICON;
Rectangle_2d MOVE_SOUTH_ICON;
Rectangle_2d MOVE_EAST_ICON;
Rectangle_2d MOVE_WEST_ICON;
Rectangle_2d MOVE_UP_ICON;
Rectangle_2d MOVE_DOWN_ICON;


std::string					CURRENT_LEVEL_STRING;
std::map <int, std::string>	LEVEL_STRING_TABLE;


/**
 * Draws a circular comm range on the minimap.
 * 
 * \fixme	Comm ranges haven't yet been fully decided upon so this function is way more flexible than it needs
 *			to be. Once comm ranges have been set in stone and the graphics are done, this function should be
 *			changed to do much faster table lookups instead of several expensive computations.
 * 
 *			By using a table lookup many of the paramters can be eliminated and instead just have the raster X/Y
 *			coords and the comm range itself. Something like this:
 * 
 *			\c drawRange(Rectangle_2d& clipRect, Image& src, int x, int y, int range);
 * 
 *			Table should look something like this:
 * 
 *			RANGE (KEY) |  SOURCE RECTANGLE
 */
static void drawRange(Rectangle_2d& clipRect, Image& src, int x, int y, int srcX, int srcY, int srcWidth, int srcHeight)
{
	int halfWidth = srcWidth / 2;
	int halfHeight = srcHeight / 2;

	// fixme: find a more efficient way to do this.
	int xClip = clamp(halfWidth - x, 0, halfWidth);
	int yClip = clamp(halfHeight - y, 0, halfHeight);
	int wClip = clamp((x + halfWidth) - clipRect.width(), 0, halfWidth);
	int hClip = clamp((y + halfHeight) - clipRect.height(), 0, halfHeight);

	Utility<Renderer>::get().drawSubImage(src,
		x + clipRect.x() - halfWidth + xClip,
		y + clipRect.y() - halfHeight + yClip,
		xClip + srcX,
		yClip + srcY,
		srcWidth - xClip - wClip,
		srcHeight - yClip - hClip);
}


/**
 * 
 */
static int pullFood(ResourcePool& _rp, int amount)
{
	if (amount <= _rp.food())
	{
		_rp.food(_rp.food() - amount);
		return amount;
	}
	else
	{
		int ret = _rp.food();
		_rp.food(0);
		return ret;
	}
}


/**
 * C'Tor
 * 
 * \param	sm	Site map to load.
 * \param	t	Tileset to use.
 * \param	d	Depth of the site map.
 * \param	mc	Mine Count - Number of mines to generate.
 */
GameState::GameState(const string& sm, const string& t, int d, int mc) :
	mFont("fonts/opensans-bold.ttf", 14),
	mTinyFont("fonts/opensans.ttf", 10),
	mTinyFontBold("fonts/opensans-bold.ttf", 10),
	mTileMap(new TileMap(sm, t, d, mc)),
	mBackground("sys/bg1.png"),
	mMapDisplay(sm + MAP_DISPLAY_EXTENSION),
	mHeightMap(sm + MAP_TERRAIN_EXTENSION),
	mUiIcons("ui/icons.png"),
	//mBgMusic("music/track_01.ogg"),
	mDiggerDirection(mTinyFont),
	mFactoryProduction(mTinyFont),
	mFileIoDialog(mTinyFont),
	mGameOverDialog(mTinyFont),
	mGameOptionsDialog(mTinyFont),
	mAnnouncement(mTinyFont),
	mStructureInspector(mTinyFont),
	mTileInspector(mTinyFont)
{}


/**
 * D'Tor
 */
GameState::~GameState()
{
	scrubRobotList();
	delete mTileMap;

	EventHandler& e = Utility<EventHandler>::get();
	e.activate().disconnect(this, &GameState::onActivate);
	e.keyDown().disconnect(this, &GameState::onKeyDown);
	e.mouseButtonDown().disconnect(this, &GameState::onMouseDown);
	e.mouseButtonUp().disconnect(this, &GameState::onMouseUp);
	e.mouseMotion().disconnect(this, &GameState::onMouseMove);
	e.windowResized().disconnect(this, &GameState::onWindowResized);

	Utility<Renderer>::get().setCursor(POINTER_NORMAL);
}


/**
 * 
 */
void GameState::setPopulationLevel(PopulationLevel _level)
{
	mLandersColonist = static_cast<int>(_level);
	mLandersCargo = 2;	///\todo This should be set based on difficulty level.
}


/**
 * Initialize values, the UI and set up event handling.
 */
void GameState::initialize()
{
	mReturnState = this;

	// EVENT HANDLERS
	EventHandler& e = Utility<EventHandler>::get();

	e.activate().connect(this, &GameState::onActivate);

	e.keyDown().connect(this, &GameState::onKeyDown);

	e.mouseButtonDown().connect(this, &GameState::onMouseDown);
	e.mouseButtonUp().connect(this, &GameState::onMouseUp);
	e.mouseMotion().connect(this, &GameState::onMouseMove);
	e.mouseWheel().connect(this, &GameState::onMouseWheel);

	e.windowResized().connect(this, &GameState::onWindowResized);

	// UI
	initUi();
	setupUiPositions();

	Utility<Renderer>::get().setCursor(POINTER_NORMAL);

	mPlayerResources.capacity(constants::BASE_STORAGE_CAPACITY);

	CURRENT_LEVEL_STRING = constants::LEVEL_SURFACE;

	if (LEVEL_STRING_TABLE.empty())
	{
		LEVEL_STRING_TABLE[constants::DEPTH_SURFACE] = constants::LEVEL_SURFACE;
		LEVEL_STRING_TABLE[constants::DEPTH_UNDERGROUND_1] = constants::LEVEL_UG1;
		LEVEL_STRING_TABLE[constants::DEPTH_UNDERGROUND_2] = constants::LEVEL_UG2;
		LEVEL_STRING_TABLE[constants::DEPTH_UNDERGROUND_3] = constants::LEVEL_UG3;
		LEVEL_STRING_TABLE[constants::DEPTH_UNDERGROUND_4] = constants::LEVEL_UG4;
	}

	e.textInputMode(true);

	mPopulationPool.population(&mPopulation);

	//Utility<Mixer>::get().fadeInMusic(mBgMusic);
	Utility<Renderer>::get().fadeIn(constants::FADE_SPEED);
}


/**
 * Updates the entire state of the game.
 */
State* GameState::update()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawImageStretched(mBackground, 0, 0, r.width(), r.height());

	// FIXME: Ugly / hacky
	if (mGameOverDialog.visible())
	{
		r.drawBoxFilled(0, 0, r.width(), r.height(), 0, 0, 0, 255);
		mGameOverDialog.update();

		if (r.isFading()) { return this; }

		return mReturnState;
	}

	// explicit current level
	r.drawText(mFont, CURRENT_LEVEL_STRING, r.width() - mFont.width(CURRENT_LEVEL_STRING) - 5, mMiniMapBoundingBox.y() - mFont.height() - mTinyFontBold.height() - 12, 255, 255, 255);
	if (mDebug) { drawDebug(); }

	if (!mGameOptionsDialog.visible() && !mGameOverDialog.visible() && !mFileIoDialog.visible())
	{
		mTileMap->injectMouse(mMousePosition.x(), mMousePosition.y());
	}

	mTileMap->draw();

	// FIXME: Ugly / hacky
	if (mGameOptionsDialog.visible() || mFileIoDialog.visible())
	{
		r.drawBoxFilled(0, 0, r.width(), r.height(), 0, 0, 0, 165);
	}

	drawUI();

	if (r.isFading()) { return this; }

	return mReturnState;
}


/**
 * Draws the minimap and all icons/overlays for it.
 */
void GameState::drawMiniMap()
{
	Renderer& r = Utility<Renderer>::get();

	if (mBtnToggleHeightmap.toggled()) { r.drawImage(mHeightMap, mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y()); }
	else { r.drawImage(mMapDisplay, mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y()); }

	if (mCCLocation.x() != 0 && mCCLocation.y() != 0)
	{
		drawRange(mMiniMapBoundingBox, mUiIcons, mCCLocation.x(), mCCLocation.y(), 166, 226, 30, 30);
		r.drawBoxFilled(mCCLocation.x() + mMiniMapBoundingBox.x() - 1, mCCLocation.y() + mMiniMapBoundingBox.y() - 1, 3, 3, 255, 255, 255);
	}

	for (auto _tower : mStructureManager.structureList(Structure::CLASS_COMM))
	{
		if (_tower->operational())
		{
			Tile* t = mStructureManager.tileFromStructure(_tower);
			drawRange(mMiniMapBoundingBox, mUiIcons, t->x(), t->y(), 146, 236, 20, 20);
		}
	}

	for(size_t i = 0; i < mTileMap->mineLocations().size(); i++)
	{
		if (mTileMap->getTile(mTileMap->mineLocations()[i].x(), mTileMap->mineLocations()[i].y(), 0)->mine()->active())
		{
			r.drawSubImage(mUiIcons, mTileMap->mineLocations()[i].x() + mMiniMapBoundingBox.x() - 2, mTileMap->mineLocations()[i].y() + mMiniMapBoundingBox.y() - 2, 8.0f, 0.0f, 7.0f, 7.0f);
		}
		else
		{
			r.drawSubImage(mUiIcons, mTileMap->mineLocations()[i].x() + mMiniMapBoundingBox.x() - 2, mTileMap->mineLocations()[i].y() + mMiniMapBoundingBox.y() - 2, 0.0f, 0.0f, 7.0f, 7.0f);
		}
	}

	for (auto it = mRobotList.begin(); it != mRobotList.end(); ++it)
	{
		r.drawPoint(it->second->x() + mMiniMapBoundingBox.x(), it->second->y() + mMiniMapBoundingBox.y(), 0, 255, 255);
	}

	r.drawBox(mMiniMapBoundingBox.x() + mTileMap->mapViewLocation().x() + 1, mMiniMapBoundingBox.y() + mTileMap->mapViewLocation().y() + 1, mTileMap->edgeLength(), mTileMap->edgeLength(), 0, 0, 0, 180);
	r.drawBox(mMiniMapBoundingBox.x() + mTileMap->mapViewLocation().x(), mMiniMapBoundingBox.y() + mTileMap->mapViewLocation().y(), mTileMap->edgeLength(), mTileMap->edgeLength(), 255, 255, 255);
}


/**
 * Convenience function to get the amount of food currently in storage.
 */
int GameState::foodInStorage()
{
	int food_count = 0;

	auto sl = mStructureManager.structureList(Structure::CLASS_FOOD_PRODUCTION);
	for (size_t i = 0; i < sl.size(); ++i)
	{
		if (sl[i]->operational() || sl[i]->isIdle())
		{
			food_count += sl[i]->storage().food();
		}
	}

	food_count += mPlayerResources.food();

	return food_count;
}


/**
 * Convenience function to get the total amount of food storage.
 */
int GameState::foodTotalStorage()
{
	int food_storage = 0;

	// Command Center has a limited amount of food storage for when colonists first land.
	if (mCCLocation.x() != 0)
	{
		food_storage += constants::BASE_STORAGE_CAPACITY;
	}

	auto sl = mStructureManager.structureList(Structure::CLASS_FOOD_PRODUCTION);
	for (size_t i = 0; i < sl.size(); ++i)
	{
		if (sl[i]->operational() || sl[i]->isIdle())
		{
			food_storage += AGRIDOME_CAPACITY;
		}
	}

	return food_storage;
}


/**
 * Draws the resource information bar.
 */
void GameState::drawResourceInfo()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(0, 0, r.width(), constants::RESOURCE_ICON_SIZE + 4, 39, 39, 39);
	r.drawBox(0, 0, r.width(), constants::RESOURCE_ICON_SIZE + 4, 21, 21, 21);
	r.drawLine(1, 0, r.width() - 2, 0, 56, 56, 56);
	
	// Resources
	int x = constants::MARGIN_TIGHT;
	int y = constants::MARGIN_TIGHT;

	int textY = 6;
	int offsetX = constants::RESOURCE_ICON_SIZE + 40;
	int margin = constants::RESOURCE_ICON_SIZE + constants::MARGIN;

	// Refined Resources
	r.drawSubImage(mUiIcons, x, y , 64, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i", mPlayerResources.commonMetals()), x + margin, textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, x + offsetX, y, 80, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i", mPlayerResources.rareMetals()), (x + offsetX) + margin, textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, (x + offsetX) * 2, y, 96, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i", mPlayerResources.commonMinerals()), (x + offsetX) * 2 + margin, textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, (x + offsetX) * 3, y, 112, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i", mPlayerResources.rareMinerals()), (x + offsetX) * 3 + margin, textY, 255, 255, 255);

	// Storage Capacity
	r.drawSubImage(mUiIcons, (x + offsetX) * 4, y, 96, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i/%i", mPlayerResources.currentLevel(), mPlayerResources.capacity()), (x + offsetX) * 4 + margin, textY, 255, 255, 255);

	// Food
	r.drawSubImage(mUiIcons, (x + offsetX) * 6, y, 64, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i/%i", foodInStorage(), foodTotalStorage()), (x + offsetX) * 6 + margin, textY, 255, 255, 255);

	// Energy
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 80, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i/%i", mPlayerResources.energy(), mStructureManager.totalEnergyProduction()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);

	// Population / Morale
	if (mCurrentMorale > mPreviousMorale)
	{
		r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 16, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	}
	else if (mCurrentMorale < mPreviousMorale)
	{
		r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 0, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	}
	else
	{
		r.drawSubImage(mUiIcons, (x + offsetX) * 10 - 17, y, 32, 48, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	}

	r.drawSubImage(mUiIcons, (x + offsetX) * 10, y, 176 + (clamp(mCurrentMorale, 1, 999) / 200) * constants::RESOURCE_ICON_SIZE, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i", mPopulation.size()), (x + offsetX) * 10 + margin, textY, 255, 255, 255);

	if (isPointInRect(mMousePosition.x(), mMousePosition.y(), 580, 0, 35, 20))
	{
		mPopulationPanel.update();
	}

	// Turns
	r.drawSubImage(mUiIcons, r.width() - 80, y, 128, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	r.drawText(mTinyFont, string_format("%i", mTurnCount), r.width() - 80 + margin, textY, 255, 255, 255);

	// ugly
	if (isPointInRect(mMousePosition, MENU_ICON))
	{
		r.drawSubImage(mUiIcons, MENU_ICON.x() + constants::MARGIN_TIGHT, MENU_ICON.y() + constants::MARGIN_TIGHT, 144, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	}
	else
	{
		r.drawSubImage(mUiIcons, MENU_ICON.x() + constants::MARGIN_TIGHT, MENU_ICON.y() + constants::MARGIN_TIGHT, 128, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	}
}


/**
 * Draws robot deployment information.
 */
void GameState::drawRobotInfo()
{
	Renderer& r = Utility<Renderer>::get();

	int x = constants::MARGIN_TIGHT;
	int y = constants::MARGIN_TIGHT;
	int textY = 6;
	int offsetX = constants::RESOURCE_ICON_SIZE + 40;
	int margin = constants::RESOURCE_ICON_SIZE + constants::MARGIN;

	// Robots
	// Start from the bottom - The bottom UI Height - Icons Height - 8 (1 offset to avoid the last to be glued with at the border)
	y = (int)r.height() - constants::BOTTOM_UI_HEIGHT - 25 - 8;
	textY = y + 7;	// Same position + 10 to center the text with the graphics
	margin = 30;	// Margin of 28 px from the graphics to the text
	x = 0; offsetX = 1;	// Start a the left side of the screen + an offset of 1 to detatch from the border
						// Miner (last one)
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 231, 18, 25, 25);
	r.drawText(mTinyFont, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_MINER), mRobotPool.miners().size()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
	// Dozer (Midle one)
	textY -= 25; y -= 25;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 206, 18, 25, 25);
	r.drawText(mTinyFont, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DOZER), mRobotPool.dozers().size()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
	// Digger (First one)
	textY -= 25; y -= 25;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 181, 18, 25, 25);
	r.drawText(mTinyFont, string_format("%i/%i", mRobotPool.getAvailableCount(ROBOT_DIGGER), mRobotPool.diggers().size()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
	// robot control summary
	textY -= 25; y -= 25;
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 231, 43, 25, 25);
	r.drawText(mTinyFont, string_format("%i/%i", mRobotPool.currentControlCount(), mRobotPool.robotControlMax()), (x + offsetX) * 8 + margin, textY, 255, 255, 255);
}


/**
 * Draws navigation UI.
 */
void GameState::drawNavInfo()
{
	Renderer& r = Utility<Renderer>::get();

	// Up / Down
	if (isPointInRect(mMousePosition, MOVE_DOWN_ICON))
		r.drawSubImage(mUiIcons, MOVE_DOWN_ICON.x(), MOVE_DOWN_ICON.y(), 64, 128, 32, 32, 255, 0, 0, 255);
	else
		r.drawSubImage(mUiIcons, MOVE_DOWN_ICON.x(), MOVE_DOWN_ICON.y(), 64, 128, 32, 32);

	if (isPointInRect(mMousePosition, MOVE_UP_ICON))
		r.drawSubImage(mUiIcons, MOVE_UP_ICON.x(), MOVE_UP_ICON.y(), 96, 128, 32, 32, 255, 0, 0, 255);
	else
		r.drawSubImage(mUiIcons, MOVE_UP_ICON.x(), MOVE_UP_ICON.y(), 96, 128, 32, 32);

	// East / West / North / South
	if (isPointInRect(mMousePosition, MOVE_EAST_ICON))
		r.drawSubImage(mUiIcons, MOVE_EAST_ICON.x(), MOVE_EAST_ICON.y(), 32, 128, 32, 16, 255, 0, 0, 255);
	else
		r.drawSubImage(mUiIcons, MOVE_EAST_ICON.x(), MOVE_EAST_ICON.y(), 32, 128, 32, 16);

	if (isPointInRect(mMousePosition, MOVE_WEST_ICON))
		r.drawSubImage(mUiIcons, MOVE_WEST_ICON.x(), MOVE_WEST_ICON.y(), 32, 144, 32, 16, 255, 0, 0, 255);
	else
		r.drawSubImage(mUiIcons, MOVE_WEST_ICON.x(), MOVE_WEST_ICON.y(), 32, 144, 32, 16);

	if (isPointInRect(mMousePosition, MOVE_NORTH_ICON))
		r.drawSubImage(mUiIcons, MOVE_NORTH_ICON.x(), MOVE_NORTH_ICON.y(), 0, 128, 32, 16, 255, 0, 0, 255);
	else
		r.drawSubImage(mUiIcons, MOVE_NORTH_ICON.x(), MOVE_NORTH_ICON.y(), 0, 128, 32, 16);

	if (isPointInRect(mMousePosition, MOVE_SOUTH_ICON))
		r.drawSubImage(mUiIcons, MOVE_SOUTH_ICON.x(), MOVE_SOUTH_ICON.y(), 0, 144, 32, 16, 255, 0, 0, 255);
	else
		r.drawSubImage(mUiIcons, MOVE_SOUTH_ICON.x(), MOVE_SOUTH_ICON.y(), 0, 144, 32, 16);


	// display the levels "bar"
	string sLevel;

	int iWidth = mTinyFontBold.width("IX");								// set steps character patern width
	int iPosX = r.width() - 5;											// set start position from right border
	int iPosY = mMiniMapBoundingBox.y() - mTinyFontBold.height() - 10;	// set vertical position
	
	for (int i = mTileMap->maxDepth(); i >= 0; i--)					//
	{
		sLevel = string_format("%i", i);	// Set string for current level
		if (i == 0) sLevel = "S";			// surface level
		if (i == mTileMap->currentDepth())
		{
			r.drawText(mTinyFontBold, sLevel, iPosX - mTinyFontBold.width(sLevel), iPosY, 255, 0, 0);		// Others in white
		}
		else
		{
			r.drawText(mTinyFontBold, sLevel, iPosX - mTinyFontBold.width(sLevel), iPosY, 200, 200, 200);	// current one in red
		}

		iPosX = iPosX - iWidth;				// Shift position by one step left
	}
}


/**
 * Displays debug information.
 */
void GameState::drawDebug()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawText(mFont, string_format("FPS: %i", mFps.fps()), 10, 25, 255, 255, 255);
	r.drawText(mFont, string_format("Map Dimensions: %i, %i", mTileMap->width(), mTileMap->height()), 10, 25 + mFont.height(), 255, 255, 255);
	r.drawText(mFont, string_format("Max Digging Depth: %i", mTileMap->maxDepth()), 10, 25 + mFont.height() * 2, 255, 255, 255);
	r.drawText(mFont, string_format("Map Mouse Hover Coords: %i, %i", mTileMap->tileMouseHoverX(), mTileMap->tileMouseHoverY()), 10, 25 + mFont.height() * 3, 255, 255, 255);
	r.drawText(mFont, string_format("Current Depth: %i", mTileMap->currentDepth()), 10, 25 + mFont.height() * 4, 255, 255, 255);

	r.drawText(mFont, string_format("Structure Count: %i", mStructureManager.count()), 10, 25 + mFont.height() * 6, 255, 255, 255);
}


/**
 * Window activation handler.
 */
void GameState::onActivate(bool _b)
{
	mLeftButtonDown = false;
}


/**
 *
 */
void GameState::onWindowResized(int w, int h)
{
	setupUiPositions();
	mTileMap->initMapDrawParams();
}


/**
 * Key down event handler.
 */
void GameState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat)
{
	if (key == EventHandler::KEY_F11)
	{
		Utility<Renderer>::get().fullscreen(!Utility<Renderer>::get().fullscreen());
		return;
	}

	// FIXME: Ugly / hacky
	if (mGameOverDialog.visible() || mFileIoDialog.visible() || mGameOptionsDialog.visible())
	{
		return;
	}

	bool viewUpdated = false; // don't like flaggy code like this
	Point_2d pt = mTileMap->mapViewLocation();

	switch(key)
	{
		case EventHandler::KEY_w:
		case EventHandler::KEY_UP:
			viewUpdated = true;
			pt.y(clamp(--pt.y(), 0, mTileMap->height() - mTileMap->edgeLength()));
			break;

		case EventHandler::KEY_s:
		case EventHandler::KEY_DOWN:
			viewUpdated = true;
			pt.y(clamp(++pt.y(), 0, mTileMap->height() - mTileMap->edgeLength()));
			break;

		case EventHandler::KEY_a:
		case EventHandler::KEY_LEFT:
			viewUpdated = true;
			pt.x(clamp(--pt.x(), 0, mTileMap->width() - mTileMap->edgeLength()));
			break;

		case EventHandler::KEY_d:
		case EventHandler::KEY_RIGHT:
			viewUpdated = true;
			pt.x(clamp(++pt.x(), 0, mTileMap->width() - mTileMap->edgeLength()));
			break;

		case EventHandler::KEY_0:
			viewUpdated = true;
			changeDepth(0);
			break;

		case EventHandler::KEY_1:
			viewUpdated = true;
			changeDepth(1);
			break;

		case EventHandler::KEY_2:
			viewUpdated = true;
			changeDepth(2);
			break;

		case EventHandler::KEY_3:
			viewUpdated = true;
			changeDepth(3);
			break;

		case EventHandler::KEY_4:
			viewUpdated = true;
			changeDepth(4);
			break;

		case EventHandler::KEY_PAGEUP:
			viewUpdated = true;
			changeDepth(mTileMap->currentDepth() - 1);
			break;

		case EventHandler::KEY_PAGEDOWN:
			viewUpdated = true;
			changeDepth(mTileMap->currentDepth() + 1);
			break;


		case EventHandler::KEY_HOME:
			viewUpdated = true;
			changeDepth(0);
			break;

		case EventHandler::KEY_END:
			viewUpdated = true;
			changeDepth(mTileMap->maxDepth());
			break;

		case EventHandler::KEY_F10:
			mDebug = !mDebug;
			break;

		case EventHandler::KEY_F2:
			mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
			mFileIoDialog.setMode(FileIo::FILE_SAVE);
			mFileIoDialog.show();
			break;

		case EventHandler::KEY_F3:
			mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
			mFileIoDialog.setMode(FileIo::FILE_LOAD);
			mFileIoDialog.show();
			break;

		case EventHandler::KEY_ESCAPE:
			clearMode();
			resetUi();
			break;

		case EventHandler::KEY_ENTER:
			if (mBtnTurns.enabled()) { nextTurn(); }
			break;

		default:
			break;
	}

	if (viewUpdated)
	{
		mTileMap->mapViewLocation(pt.x(), pt.y());
	}
}


/**
 * Mouse Down event handler.
 */
void GameState::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	// FIXME: Ugly / hacky
	if (mGameOverDialog.visible() || mFileIoDialog.visible() || mGameOptionsDialog.visible()) { return; }

	if (mDiggerDirection.visible() && isPointInRect(mMousePosition, mDiggerDirection.rect())) { return; }

	if (mWindowStack.pointInWindow(mMousePosition) && button == EventHandler::BUTTON_LEFT)
	{
		mWindowStack.updateStack(mMousePosition);
		return;
	}

	if (button == EventHandler::BUTTON_RIGHT)
	{
		Tile* _t = mTileMap->getTile(mTileMap->tileHighlight().x() + mTileMap->mapViewLocation().x(), mTileMap->tileHighlight().y() + mTileMap->mapViewLocation().y());

		if (mInsertMode != INSERT_NONE)
		{
			resetUi();
			return;
		}

		if (!mTileMap->tileHighlightVisible())
			return;

		if (!_t)
		{
			return;
		}
		else if (_t->empty() && isPointInRect(mMousePosition, mTileMap->boundingBox()))
		{
			clearSelections();
			mTileInspector.tile(_t);
			mTileInspector.show();
			mWindowStack.bringToFront(&mTileInspector);
		}
		else if (_t->thingIsStructure())
		{
			if (_t->structure()->isFactory() && (_t->structure()->operational() || _t->structure()->isIdle()))
			{
				mFactoryProduction.factory(static_cast<Factory*>(_t->structure()));
				mFactoryProduction.show();
				mWindowStack.bringToFront(&mFactoryProduction);
			}
			else
			{
				mStructureInspector.structure(_t->structure());
				mStructureInspector.show();
				mWindowStack.bringToFront(&mStructureInspector);
			}
		}
	}

	if (button == EventHandler::BUTTON_LEFT)
	{
		mLeftButtonDown = true;

		mWindowStack.updateStack(mMousePosition);
		Point_2d pt = mTileMap->mapViewLocation();

		// Ugly
		if (isPointInRect(mMousePosition, MENU_ICON))
		{
			mGameOptionsDialog.show();
		}

		if (isPointInRect(mMousePosition, MOVE_NORTH_ICON))
		{
			pt.y(clamp(--pt.y(), 0, mTileMap->height() - mTileMap->edgeLength()));
			mTileMap->mapViewLocation(pt.x(), pt.y());
		}
		else if (isPointInRect(mMousePosition, MOVE_SOUTH_ICON))
		{
			pt.y(clamp(++pt.y(), 0, mTileMap->height() - mTileMap->edgeLength()));
			mTileMap->mapViewLocation(pt.x(), pt.y());
		}
		else if (isPointInRect(mMousePosition, MOVE_EAST_ICON))
		{
			pt.x(clamp(++pt.x(), 0, mTileMap->width() - mTileMap->edgeLength()));
			mTileMap->mapViewLocation(pt.x(), pt.y());
		}
		else if (isPointInRect(mMousePosition, MOVE_WEST_ICON))
		{
			pt.x(clamp(--pt.x(), 0, mTileMap->width() - mTileMap->edgeLength()));
			mTileMap->mapViewLocation(pt.x(), pt.y());
		}
		else if (isPointInRect(mMousePosition, MOVE_UP_ICON))
		{
			changeDepth(mTileMap->currentDepth() - 1);
		}
		else if (isPointInRect(mMousePosition, MOVE_DOWN_ICON))
		{
			changeDepth(mTileMap->currentDepth()+1);
		}

		// MiniMap Check
		if (isPointInRect(mMousePosition, mMiniMapBoundingBox) && !mWindowStack.pointInWindow(mMousePosition))
		{
			setMinimapView();
		}
		// Click was within the bounds of the TileMap.
		else if (isPointInRect(mMousePosition, mTileMap->boundingBox()))
		{
			if (mInsertMode == INSERT_STRUCTURE)
			{
				placeStructure();
			}
			else if (mInsertMode == INSERT_ROBOT)
			{
				placeRobot();
			}
			else if (mInsertMode == INSERT_TUBE)
			{
				placeTubes();
			}
		}
	}
}


/**
* Mouse Up event handler.
*/
void GameState::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	if (button == EventHandler::BUTTON_LEFT)
	{
		mLeftButtonDown = false;
	}
}


/**
* Mouse motion event handler.
*/
void GameState::onMouseMove(int x, int y, int rX, int rY)
{
	mMousePosition(x, y);

	if (mLeftButtonDown)
	{
		if (isPointInRect(mMousePosition, mMiniMapBoundingBox))
		{
			setMinimapView();
		}
	}

	mTileMapMouseHover(mTileMap->tileMouseHoverX(), mTileMap->tileMouseHoverY());
}


/**
 * Mouse wheel event handler.
 */
void GameState::onMouseWheel(int x, int y)
{
	if (mInsertMode != INSERT_TUBE) { return; }

	if (y > 0) { mConnections.decrementSelection(); }
	else { mConnections.incrementSelection(); }
}


/**
 * Changes the current view depth.
 */
bool GameState::changeDepth(int _d)
{
	int mPrevious = mTileMap->currentDepth();
	mTileMap->currentDepth(_d);

	if (mTileMap->currentDepth() == mPrevious) { return false; }

	clearMode();
	populateStructureMenu();
	updateCurrentLevelString(mTileMap->currentDepth());
	return true;
}


/**
 * 
 */
void GameState::setMinimapView()
{
	int x = clamp(mMousePosition.x() - mMiniMapBoundingBox.x() - mTileMap->edgeLength() / 2, 0, mTileMap->width() - mTileMap->edgeLength());
	int y = clamp(mMousePosition.y() - mMiniMapBoundingBox.y() - mTileMap->edgeLength() / 2, 0, mTileMap->height() - mTileMap->edgeLength());

	mTileMap->mapViewLocation(x, y);
}


/**
 * Clears the build mode.
 */
void GameState::clearMode()
{
	mInsertMode = INSERT_NONE;
	Utility<Renderer>::get().setCursor(POINTER_NORMAL);

	mCurrentStructure = SID_NONE;
	mCurrentRobot = ROBOT_NONE;

	clearSelections();
}


/**
 * 
 */
void GameState::insertTube(ConnectorDir _dir, int _depth, Tile* _t)
{
	if (_dir == CONNECTOR_INTERSECTION)
	{
		mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, _depth != 0), _t);
	}
	else if (_dir == CONNECTOR_RIGHT)
	{
		mStructureManager.addStructure(new Tube(CONNECTOR_RIGHT, _depth != 0), _t);
	}
	else if (_dir == CONNECTOR_LEFT)
	{
		mStructureManager.addStructure(new Tube(CONNECTOR_LEFT, _depth != 0), _t);
	}
	else
	{
		throw std::runtime_error("GameState::placeTube() called but Current Structure is not a tube!");
	}
}


/**
 * 
 */
void GameState::placeTubes()
{
	int x = mTileMapMouseHover.x();
	int y = mTileMapMouseHover.y();

	Tile* tile = mTileMap->getVisibleTile(x, y, mTileMap->currentDepth());
	if (!tile) { return; }

	// Check the basics.
	if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated()) { return; }

	ConnectorDir cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);
	if (validTubeConnection(mTileMap, x, y, cd))
	{
		insertTube(cd, mTileMap->currentDepth(), mTileMap->getTile(x, y));

		// FIXME: Naive approach -- will be slow with larger colonies.
		mStructureManager.disconnectAll();
		checkConnectedness();
	}
	else
	{
		Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_TUBE_PLACEMENT);
	}
}


/**
 * 
 */
void GameState::placeRobot()
{
	Tile* tile = mTileMap->getVisibleTile();
	if (!tile) { return; }
	if (!mRobotPool.robotCtrlAvailable()) { return; }
	
	// NOTE:	This function will never be called until the seed lander is deployed so there
	//			is no need to check that the CC Location is anything other than { 0, 0 }.
	if (outOfCommRange(mStructureManager, mCCLocation, mTileMap, tile))
	{
		cout << "Robot out of range!" << endl;
		Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::OUT_OF_COMM_RANGE);
		return;
	}

	// Robodozer has been selected.
	if(mCurrentRobot == ROBOT_DOZER)
	{
		if (tile->mine() || !tile->excavated() || (tile->thing() && !tile->thingIsStructure()))
		{
			return;
		}
		else if (tile->thingIsStructure())
		{
			Structure* _s = tile->structure();
			if (_s->name() == constants::COMMAND_CENTER)
			{
				Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::CC_NO_BULLDOZE);
				cout << "Can't bulldoze a Command Center!" << endl;
				return;
			}

			if (_s->name() == constants::COLONIST_LANDER && _s->age() == 0)
			{
				Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::LANDER_NO_BULLDOZE); ///\fixme Change this to an invalid dozer warning.
				cout << "Can't place a bulldozer on a landing site!" << endl;
				return;
			}

			ResourcePool resPool = StructureCatalogue::recyclingValue(StructureTranslator::translateFromString(_s->name()));
			mPlayerResources.pushResources(resPool);

			tile->connected(false);
			mStructureManager.removeStructure(_s);
			tile->deleteThing();
			mStructureManager.disconnectAll();
			checkConnectedness();
		}
		else if (tile->index() == TERRAIN_DOZED)
		{
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::TILE_BULLDOZED);
			return;
		}

		Robot* r = mRobotPool.getDozer();
		r->startTask(tile->index());
		mRobotPool.insertRobotIntoTable(mRobotList, r, tile);
		tile->index(TERRAIN_DOZED);

		if(!mRobotPool.robotAvailable(ROBOT_DOZER))
		{
			mRobots.removeItem(constants::ROBODOZER);
			clearMode();
		}
	}
	// Robodigger has been selected.
	else if(mCurrentRobot == ROBOT_DIGGER)
	{
		// Keep digger within a safe margin of the map boundaries.
		if (mTileMapMouseHover.x() < 3 || mTileMapMouseHover.x() > mTileMap->width() - 4 || mTileMapMouseHover.y() < 3 || mTileMapMouseHover.y() > mTileMap->height() - 4)
		{
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_DIGGER_PLACEMENT);
			cout << "GameState::placeRobot(): Can't place digger within 3 tiles of the edge of a map." << endl;
			return;
		}

		if (!tile->excavated())
		{
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_DIGGER_PLACEMENT);
			return;
		}

		// Check for obstructions underneath the the digger location.
		if (tile->depth() != mTileMap->maxDepth() && !mTileMap->getTile(tile->x(), tile->y(), tile->depth() + 1)->empty())
		{
			cout << "Digger blocked underneath." << endl;
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_DIGGER_PLACEMENT);
			return;
		}

		// Die if tile is occupied or not excavated.
		if (!tile->empty())
		{

			if (tile->depth() > constants::DEPTH_SURFACE)
			{
				if (tile->thingIsStructure() && tile->structure()->connectorDirection() != CONNECTOR_VERTICAL) //air shaft
				{
					Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_DIGGER_PLACEMENT);
					return;
				}
				else if (tile->thingIsStructure() && tile->structure()->connectorDirection() == CONNECTOR_VERTICAL && tile->depth() == mTileMap->maxDepth())
				{
					Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::MAX_DIGGING_DEPTH_REACHED);
					return;
				}
			}
			else
			{
				Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_DIGGER_PLACEMENT); // tile occupied
				return;
			}
		}

		if (!tile->thing() && mTileMap->currentDepth() > 0) { mDiggerDirection.cardinalOnlyEnabled(); }
		else { mDiggerDirection.downOnlyEnabled(); }

		mDiggerDirection.setParameters(tile);

		// NOTE:	Unlike the Dozer and Miner, Digger's aren't removed here but instead
		//			are removed after responses to the DiggerDirection dialog.

		// If we're placing on the top level we can only ever go down.
		if (mTileMap->currentDepth() == constants::DEPTH_SURFACE)
		{
			mDiggerDirection.selectDown();
		}
		else
		{
			mDiggerDirection.show();
			mWindowStack.bringToFront(&mDiggerDirection);

			int x = mMousePosition.x() + 20;

			if (x + mDiggerDirection.width() > Utility<Renderer>::get().width())
			{
				x = mMousePosition.x() - mDiggerDirection.width() - 20;
			}

			mDiggerDirection.position(x, mMousePosition.y() - 32);
		}
	}
	// Robominer has been selected.
	else if(mCurrentRobot == ROBOT_MINER)
	{
		if (tile->thing() || !tile->mine() || !tile->excavated())
		{
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_MINER_PLACEMENT);
			return;
		}

		Robot* r = mRobotPool.getMiner();
		r->startTask(6);
		mRobotPool.insertRobotIntoTable(mRobotList, r, tile);
		tile->index(TERRAIN_DOZED);

		if (!mRobotPool.robotAvailable(ROBOT_MINER))
		{
			mRobots.removeItem(constants::ROBOMINER);
			clearMode();
		}
	}
}


/**
 * Checks the robot selection interface and if the robot is not available in it, adds
 * it back in and reeneables the robots button if it's not enabled.
 */
void GameState::checkRobotSelectionInterface(const std::string& rType, int sheetIndex)
{
	if (!mRobots.itemExists(rType))
	{
		mRobots.addItem(rType, sheetIndex);
	}
}


/**
 * Called whenever a RoboDozer completes its task.
 */
void GameState::dozerTaskFinished(Robot* _r)
{
	checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID);
}


/**
 * Called whenever a RoboDigger completes its task.
 */
void GameState::diggerTaskFinished(Robot* _r)
{
	if (mRobotList.find(_r) == mRobotList.end()) { throw std::runtime_error("GameState::diggerTaskFinished() called with a Robot not in the Robot List!"); }

	Tile* t = mRobotList[_r];

	if (t->depth() > mTileMap->maxDepth())
	{
		throw std::runtime_error("Digger defines a depth that exceeds the maximum digging depth!");
	}

	// FIXME: Fugly cast.
	Direction dir = static_cast<Robodigger*>(_r)->direction();

	int originX = 0, originY = 0, depthAdjust = 0;

	if(dir == DIR_DOWN)
	{
		AirShaft* as1 = new AirShaft();
		if (t->depth() > 0) { as1->ug(); }
		mStructureManager.addStructure(as1, t);

		AirShaft* as2 = new AirShaft();
		as2->ug();
		mStructureManager.addStructure(as2, mTileMap->getTile(t->x(), t->y(), t->depth() + 1));

		originX = t->x();
		originY = t->y();
		depthAdjust = 1;

		mTileMap->getTile(originX, originY, t->depth())->index(TERRAIN_DOZED);
		mTileMap->getTile(originX, originY, t->depth() + depthAdjust)->index(TERRAIN_DOZED);

		// FIXME: Naive approach; will be slow with large colonies.
		mStructureManager.disconnectAll();
		checkConnectedness();
	}
	else if(dir == DIR_NORTH)
	{
		originX = t->x();
		originY = t->y() - 1;
	}
	else if(dir == DIR_SOUTH)
	{
		originX = t->x();
		originY = t->y() + 1;
	}
	else if(dir == DIR_WEST)
	{
		originX = t->x() - 1;
		originY = t->y();
	}
	else if(dir == DIR_EAST)
	{
		originX = t->x() + 1;
		originY = t->y();
	}

	/**
	 * \todo	Add checks for obstructions and things that explode if
	 *			a digger gets in the way (or should diggers be smarter than
	 *			puncturing a fusion reactor containment vessel?)
	 */
	for(int y = originY - 1; y <= originY + 1; ++y)
	{
		for(int x = originX - 1; x <= originX + 1; ++x)
		{
			mTileMap->getTile(x, y, t->depth() + depthAdjust)->excavated(true);
		}
	}

	checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID);
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void GameState::minerTaskFinished(Robot* _r)
{
	if (mRobotList.find(_r) == mRobotList.end()) { throw std::runtime_error("GameState::minerTaskFinished() called with a Robot not in the Robot List!"); }

	Tile* t = mRobotList[_r];

	if (t->depth() == constants::DEPTH_SURFACE)
	{
		mStructureManager.addStructure(new MineFacility(t->mine()), t);
	}
	else
	{
		mStructureManager.addStructure(new MineShaft(), t);
	}

	Tile* t2 = mTileMap->getTile(t->x(), t->y(), t->depth() + 1);
	mStructureManager.addStructure(new MineShaft(), t2);

	t->index(0);
	t2->index(0);
	t2->excavated(true);

	checkRobotSelectionInterface(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID);
}


/**
 * Called whenever a Factory's production is complete.
 */
void GameState::factoryProductionComplete(ProductType _p, int id)
{
	cout << "Factory '" << id << "' has finished producing a";
	
	switch (_p)
	{
	case PRODUCT_DIGGER:
		cout << " RoboDigger" << endl;
		mRobotPool.addRobot(ROBOT_DIGGER)->taskComplete().connect(this, &GameState::diggerTaskFinished);
		break;

	case PRODUCT_DOZER:
		cout << " RoboDozer" << endl;
		mRobotPool.addRobot(ROBOT_DOZER)->taskComplete().connect(this, &GameState::dozerTaskFinished);
		break;

	case PRODUCT_MINER:
		cout << " RoboMiner" << endl;
		mRobotPool.addRobot(ROBOT_MINER)->taskComplete().connect(this, &GameState::minerTaskFinished);
		break;

	default:
		cout << "n Unknown Product." << endl;
		break;
	}
}


/**
 * Places a structure into the map.
 */
void GameState::placeStructure()
{
	// SID_NONE is a logic error and should fail as loudly as possible.
	if (mCurrentStructure == SID_NONE) { throw std::runtime_error("GameState::placeStructure() called but mCurrentStructure == STRUCTURE_NONE"); }

	Tile* tile = mTileMap->getVisibleTile();
	if (!tile) { return; }

	// NOTE:	This function will never be called until the seed lander is deployed so there
	//			is no need to check that the CC Location is anything other than { 0, 0 }.
	if (!structureIsLander(mCurrentStructure) && !selfSustained(mCurrentStructure) &&
		(tile->distanceTo(mTileMap->getTile(mCCLocation.x(), mCCLocation.y(), 0)) > constants::ROBOT_COM_RANGE))
	{
		cout << "Cannot build structures more than " << constants::ROBOT_COM_RANGE << " tiles away from Command Center." << endl;
		Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_STRUCTURE_PLACEMENT);
		return;
	}

	if(tile->mine() || tile->thing() || (!tile->bulldozed() && !structureIsLander(mCurrentStructure)))
	{
		Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_STRUCTURE_PLACEMENT);
		cout << "GameState::placeStructure(): Tile is unsuitable to place a structure." << endl;
		return;
	}

	int tile_x = mTileMapMouseHover.x(), tile_y = mTileMapMouseHover.y();

	// Seed lander is a special case and only one can ever be placed by the player ever.
	if(mCurrentStructure == SID_SEED_LANDER)
	{
		insertSeedLander(tile_x, tile_y);
	}
	else if (mCurrentStructure == SID_COLONIST_LANDER)
	{
		if (!validLanderSite(tile)) { return; }

		ColonistLander* s = new ColonistLander(tile);
		s->deployCallback().connect(this, &GameState::deployColonistLander);
		mStructureManager.addStructure(s, tile);

		--mLandersColonist;
		if (mLandersColonist == 0)
		{
			clearMode();
			resetUi();
			populateStructureMenu();
		}
	}
	else if (mCurrentStructure == SID_CARGO_LANDER)
	{
		if (!validLanderSite(tile)) { return; }

		CargoLander* _lander = new CargoLander(tile);
		_lander->deployCallback().connect(this, &GameState::deployCargoLander);
		mStructureManager.addStructure(_lander, tile);

		--mLandersCargo;
		if (mLandersCargo == 0)
		{
			clearMode();
			resetUi();
			populateStructureMenu();
		}
	}
	else
	{
		if (!validStructurePlacement(mTileMap, tile_x, tile_y) && !selfSustained(mCurrentStructure))
		{
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INVALID_STRUCTURE_PLACEMENT);
			cout << "GameState::placeStructure(): Invalid structure placement." << endl;
			return;
		}

		// Check build cost
		if (!StructureCatalogue::canBuild(mPlayerResources, static_cast<StructureID>(mCurrentStructure)))
		{
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INSUFFICIENT_RESOURCES);
			cout << "GameState::placeStructure(): Insufficient resources to build structure." << endl;
			return;
		}

		Structure* _s = StructureCatalogue::get(mCurrentStructure);
		if (!_s) { throw std::runtime_error("GameState::placeStructure(): NULL Structure returned from StructureCatalog."); }

		mStructureManager.addStructure(_s, tile);

		// FIXME: Ugly
		if (_s->isFactory())
		{
			static_cast<Factory*>(_s)->productionComplete().connect(this, &GameState::factoryProductionComplete);
			static_cast<Factory*>(_s)->resourcePool(&mPlayerResources);
		}

		mPlayerResources -= StructureCatalogue::costToBuild(mCurrentStructure);
	}
}


/**
 * Checks that the clicked tile is a suitable spot for the SEED Lander and
 * then inserts it into the the TileMap.
 */
void GameState::insertSeedLander(int x, int y)
{
	// Has to be built away from the edges of the map
	if (x > 3 && x < mTileMap->width() - 4 && y > 3 && y < mTileMap->height() - 4)
	{
		// check for obstructions
		if (!landingSiteSuitable(mTileMap, x, y))
		{
			Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::UNSUITABLE_LANDING_SITE);
			cout << "Unable to place SEED Lander. Tiles obstructed." << endl;
			return;
		}

		SeedLander* s = new SeedLander(x, y);
		s->deployCallback().connect(this, &GameState::deploySeedLander);
		mStructureManager.addStructure(s, mTileMap->getTile(x, y)); // Can only ever be placed on depth level 0

		clearMode();
		resetUi();

		mStructures.dropAllItems();
		mBtnTurns.enabled(true);
	}
	else
	{
		Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::UNSUITABLE_LANDING_SITE);
	}
}


/**
 * Lands colonists on the surfaces and adds them to the population pool.
 */
void GameState::deployColonistLander()
{
	mPopulation.addPopulation(Population::ROLE_STUDENT, 10);
	mPopulation.addPopulation(Population::ROLE_WORKER, 20);
	mPopulation.addPopulation(Population::ROLE_SCIENTIST, 20);
}


/**
 * Lands cargo on the surface and adds resources to the resource pool.
 */
void GameState::deployCargoLander()
{
	///\fixme Magic numbers
	mPlayerResources.commonMetals(mPlayerResources.commonMetals() + 25);
	mPlayerResources.commonMinerals(mPlayerResources.commonMinerals() + 25);
	mPlayerResources.rareMetals(mPlayerResources.rareMetals() + 15);
	mPlayerResources.rareMinerals(mPlayerResources.rareMinerals() + 15);

	mPlayerResources.food(mPlayerResources.food() + 125);
}


/**
 * Sets up the initial colony deployment.
 *
 * \note	The deploy callback only gets called once so there is really no
 *			need to disconnect the callback since it will automatically be
 *			released when the seed lander is destroyed.
 */
void GameState::deploySeedLander(int x, int y)
{
	mTileMap->getTile(x, y)->index(TERRAIN_DOZED);

	// TOP ROW
	mStructureManager.addStructure(new SeedPower(), mTileMap->getTile(x - 1, y - 1));
	mTileMap->getTile(x - 1, y - 1)->index(TERRAIN_DOZED);

	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x, y - 1));
	mTileMap->getTile(x, y - 1)->index(TERRAIN_DOZED);

	CommandCenter* cc = new CommandCenter();
	cc->sprite().skip(3);
	mStructureManager.addStructure(cc, mTileMap->getTile(x + 1, y - 1));
	mTileMap->getTile(x + 1, y - 1)->index(TERRAIN_DOZED);
	mCCLocation(x + 1, y - 1);

	// MIDDLE ROW
	mTileMap->getTile(x - 1, y)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x - 1, y));

	mTileMap->getTile(x + 1, y)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x + 1, y));

	// BOTTOM ROW
	SeedFactory* sf = new SeedFactory();
	sf->resourcePool(&mPlayerResources);
	sf->productionComplete().connect(this, &GameState::factoryProductionComplete);
	sf->sprite().skip(7);
	mStructureManager.addStructure(sf, mTileMap->getTile(x - 1, y + 1));
	mTileMap->getTile(x - 1, y + 1)->index(TERRAIN_DOZED);

	mTileMap->getTile(x, y + 1)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x, y + 1));

	SeedSmelter* ss = new SeedSmelter();
	ss->sprite().skip(10);
	mStructureManager.addStructure(ss, mTileMap->getTile(x + 1, y + 1));
	mTileMap->getTile(x + 1, y + 1)->index(TERRAIN_DOZED);

	// Robots only become available after the SEED Factor is deployed.
	mRobots.addItem(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID);
	mRobots.addItem(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID);
	mRobots.addItem(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID);

	mRobotPool.addRobot(ROBOT_DOZER)->taskComplete().connect(this, &GameState::dozerTaskFinished);
	mRobotPool.addRobot(ROBOT_DIGGER)->taskComplete().connect(this, &GameState::diggerTaskFinished);
	mRobotPool.addRobot(ROBOT_MINER)->taskComplete().connect(this, &GameState::minerTaskFinished);
}



/**
 * Updates all robots.
 */
void GameState::updateRobots()
{
	auto robot_it = mRobotList.begin();
	while(robot_it != mRobotList.end())
	{
		robot_it->first->update();

		// Clear Idle robots from tiles.
		if(robot_it->first->idle())
		{
			// Make sure that we're the robot from a Tile and not something else
			if (robot_it->second->thing() == robot_it->first)
			{
				robot_it->second->removeThing();
			}

			robot_it = mRobotList.erase(robot_it);
		}
		else
		{
			++robot_it;
		}
	}

	updateRobotControl(mRobotPool, mStructureManager);
}


/**
 * Checks and sets the current structure mode.
 */
void GameState::setStructureID(StructureID type, InsertMode mode)
{

	if (type == SID_NONE)
	{
		clearMode();
		return;
	}

	mCurrentStructure = type;

	mInsertMode = mode;
	Utility<Renderer>::get().setCursor(POINTER_PLACE_TILE);

}


/**
 * Checks the connectedness of all tiles surrounding
 * the Command Center.
 */
void GameState::checkConnectedness()
{
	if (mCCLocation.x() == 0 && mCCLocation.y() == 0)
	{
		return;
	}

	// Assumes that the 'thing' at mCCLocation is in fact a Structure.
	Tile *t = mTileMap->getTile(mCCLocation.x(), mCCLocation.y(), 0);
	Structure *cc = t->structure();

	if (!cc)
	{
		throw std::runtime_error("CC coordinates do not actually point to a Command Center.");
	}

	if (cc->state() == Structure::UNDER_CONSTRUCTION)
	{
		return;
	}

	t->connected(true);

	// Start graph walking at the CC location.
	GraphWalker graphWalker(mCCLocation, 0, mTileMap);
}


/**
 * Removes deployed robots from the TileMap to
 * prevent dangling pointers. Yay for raw memory!
 */
void GameState::scrubRobotList()
{
	for (auto it = mRobotList.begin(); it != mRobotList.end(); ++it)
	{
		it->second->removeThing();
	}
}


/**
 * Update the value of the current level string
 */
void GameState::updateCurrentLevelString(int currentDepth)
{
	CURRENT_LEVEL_STRING = LEVEL_STRING_TABLE[currentDepth];
}


/**
 * 
 */
void GameState::updatePopulation()
{
	int residences = mStructureManager.getCountInState(Structure::CLASS_RESIDENCE, Structure::OPERATIONAL);
	int universities = mStructureManager.getCountInState(Structure::CLASS_UNIVERSITY, Structure::OPERATIONAL);
	int nurseries = mStructureManager.getCountInState(Structure::CLASS_NURSERY, Structure::OPERATIONAL);
	int hospitals = mStructureManager.getCountInState(Structure::CLASS_MEDICAL_CENTER, Structure::OPERATIONAL);

	// FOOD CONSUMPTION
	int food_consumed = mPopulation.update(mCurrentMorale, foodInStorage(), residences, universities, nurseries, hospitals);
	StructureManager::StructureList &foodproducers = mStructureManager.structureList(Structure::CLASS_FOOD_PRODUCTION);
	int remainder = food_consumed;

	if (mPlayerResources.food() > 0)
	{
		remainder -= pullFood(mPlayerResources, remainder);
	}

	for (size_t i = 0; i < foodproducers.size(); ++i)
	{
		if (remainder <= 0) { break; }

		remainder -= pullFood(foodproducers[i]->storage(), remainder);
	}
}


/**
 * 
 */
void GameState::updateMorale()
{
	// POSITIVE MORALE EFFECTS
	// =========================================
	mCurrentMorale += mPopulation.birthCount();
	mCurrentMorale += mStructureManager.getCountInState(Structure::CLASS_PARK, Structure::OPERATIONAL);
	mCurrentMorale += mStructureManager.getCountInState(Structure::CLASS_RECREATION_CENTER, Structure::OPERATIONAL);

	int food_production = mStructureManager.getCountInState(Structure::CLASS_FOOD_PRODUCTION, Structure::OPERATIONAL);
	food_production > 0 ? mCurrentMorale += food_production : mCurrentMorale -= 5;

	//mCurrentMorale += mStructureManager.getCountInState(Structure::STRUCTURE_COMMERCIAL, Structure::OPERATIONAL);

	// NEGATIVE MORALE EFFECTS
	// =========================================
	mCurrentMorale -= mPopulation.deathCount();
	mCurrentMorale -= mStructureManager.disabled();
	mCurrentMorale -= mStructureManager.destroyed();

	int residentialMoraleHit = static_cast<int>(mPopulationPanel.capacity() / 100.0f);

	// Ensure that there is always a morale hit if residential capacity is more than 100%.
	if (mPopulationPanel.capacity() > 100.0f && residentialMoraleHit < constants::MINIMUM_RESIDENCE_OVERCAPACITY_HIT) { residentialMoraleHit = constants::MINIMUM_RESIDENCE_OVERCAPACITY_HIT; }

	mCurrentMorale -= residentialMoraleHit;

	mCurrentMorale = clamp(mCurrentMorale, 0, 1000);
}


/**
 * 
 */
void GameState::updateResources()
{
	// Update storage capacity
	mPlayerResources.capacity(totalStorage(mStructureManager.structureList(Structure::CLASS_STORAGE)));

	ResourcePool truck;
	truck.capacity(100);

	auto mines = mStructureManager.structureList(Structure::CLASS_MINE);
	auto smelters = mStructureManager.structureList(Structure::CLASS_SMELTER);

	// Move ore from mines to smelters
	for (size_t m = 0; m < mines.size(); ++m)
	{
		if (mines[m]->disabled() || mines[m]->destroyed())
		{
			continue; // consider a different control path.
		}

		truck.commonMetalsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, 25));
		truck.commonMineralsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, 25));
		truck.rareMetalsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_RARE_METALS_ORE, 25));
		truck.rareMineralsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, 25));

		for (size_t s = 0; s < smelters.size(); ++s)
		{
			if (smelters[s]->operational())
			{
				smelters[s]->production().pushResources(truck);
			}
		}

		if (!truck.empty())
		{
			mines[m]->storage().pushResources(truck);
		}
	}

	// Move refined resources from smelters to storage tanks
	for (size_t s = 0; s < smelters.size(); ++s)
	{
		if (smelters[s]->disabled() || smelters[s]->destroyed())
		{
			continue; // consider a different control path.
		}

		truck.commonMetals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_COMMON_METALS, 25));
		truck.commonMinerals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_COMMON_MINERALS, 25));
		truck.rareMetals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_RARE_METALS, 25));
		truck.rareMinerals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_RARE_MINERALS, 25));

		mPlayerResources.pushResources(truck);

		if (!truck.empty())
		{
			smelters[s]->storage().pushResources(truck);
			break;	// we're at max capacity in our storage, dump what's left in the smelter it came from and barf.
		}
	}
}


/**
 * Check for colony ship deorbiting; if any colonists are remaining, kill
 * them and reduce morale by an appropriate amount.
 */
void GameState::checkColonyShip()
{
	if (mTurnCount == constants::COLONY_SHIP_ORBIT_TIME)
	{
		if (mLandersColonist > 0 || mLandersCargo > 0)
		{
			mCurrentMorale -= (mLandersColonist * 50) * 6; /// \todo apply a modifier to multiplier based on difficulty level.
			if (mCurrentMorale < 0) { mCurrentMorale = 0; }

			mLandersColonist = 0;
			mLandersCargo = 0;

			populateStructureMenu();

			mWindowStack.bringToFront(&mAnnouncement);
			mAnnouncement.announcement(MajorEventAnnouncement::ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS);
			mAnnouncement.show();
		}
		else
		{
			mWindowStack.bringToFront(&mAnnouncement);
			mAnnouncement.announcement(MajorEventAnnouncement::ANNOUNCEMENT_COLONY_SHIP_CRASH);
			mAnnouncement.show();
		}
	}
}


/**
 * 
 */
void GameState::updateResidentialCapacity()
{
	mResidentialCapacity = 0;
	auto residences = mStructureManager.structureList(Structure::CLASS_RESIDENCE);
	for (auto residence : residences)
	{
		if (residence->operational()) { mResidentialCapacity += static_cast<Residence*>(residence)->capacity(); }
	}

	if (residences.empty()) { mResidentialCapacity = constants::COMMAND_CENTER_POPULATION_CAPACITY; }

	mPopulationPanel.residential_capacity(mResidentialCapacity);
}


/**
 * 
 */
void GameState::nextTurn()
{
	clearMode();

	mPopulationPool.clear();

	mStructureManager.disconnectAll();
	checkConnectedness();
	mStructureManager.update(mPlayerResources, mPopulationPool);

	mPreviousMorale = mCurrentMorale;

	updateResidentialCapacity();

	updatePopulation();
	updateMorale();
	updateRobots();

	updateResources();

	Structure* cc = mTileMap->getTile(mCCLocation.x(), mCCLocation.y(), TileMap::LEVEL_SURFACE)->structure();
	if (cc->state() == Structure::OPERATIONAL)
	{
		populateStructureMenu();
	}

	checkColonyShip();

	// Check for Game Over conditions
	if (mPopulation.size() < 1 && mLandersColonist == 0)
	{
		hideUi();
		mGameOverDialog.show();
	}

	mTurnCount++;
}
