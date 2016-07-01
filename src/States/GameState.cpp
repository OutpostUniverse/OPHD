#include "GameState.h"
#include "PlanetSelectState.h"

#include "GameStateHelper.h"


#include "../GraphWalker.h"

#include "../Tile.h"

#include "../Things/Robots/Robots.h"
#include "../Things/Structures/Structures.h"

#include "../Constants.h"
#include "../StructureFactory.h"

#include <sstream>
#include <vector>

// Disable some warnings that can be safely ignored.
#pragma warning(disable: 4244) // possible loss of data (floats to int and vice versa)


using namespace std;
using namespace NAS2D;


const std::string	MAP_TERRAIN_EXTENSION		= "_a.png";
const std::string	MAP_DISPLAY_EXTENSION		= "_b.png";

const int MAX_TILESET_INDEX	= 4;

const int MAX_DEPTH = 4;

Rectangle_2d MENU_ICON;

stringstream str_scratch;		// Used in a few places to avoid construction/destruction every frame when drawing resource stats.

/**
 * C'Tor
 */
GameState::GameState(const string& map, const string& tset):	mFont("fonts/Fresca-Regular.ttf", 14),
																mTinyFont("fonts/Fresca-Regular.ttf", 10),
																mTileMap(map, tset, MAX_DEPTH),
																mMapDisplay(map + MAP_DISPLAY_EXTENSION),
																mHeightMap(map + MAP_TERRAIN_EXTENSION),
																mUiIcons("ui/icons.png"),
																mCurrentPointer(POINTER_NORMAL),
																mCurrentStructure(SID_NONE),
																mDiggerDirection(mTinyFont),
																mTileInspector(mTinyFont),
																mFactoryProduction(mTinyFont),
																mInsertMode(INSERT_NONE),
																mTurnCount(0),
																mReturnState(NULL),
																mLeftButtonDown(false),
																mDebug(false)
{}


/**
 * D'Tor
 */
GameState::~GameState()
{
	/**
	 * Robots are managed by the Robot Pool. Remove them from
	 * Tile's before Tile decides to free memory it shouldn't
	 * be freeing.
	 * 
	 * \note	Structures are not specially handled by a
	 *			manager object so Tile can safely free those.
	 */
	RobotMap::iterator it = mRobotList.begin();
	while(it != mRobotList.end())
	{
		it->second->removeThing();
		++it;
	}

	EventHandler& e = Utility<EventHandler>::get();
	e.activate().Disconnect(this, &GameState::onActivate);
	e.keyDown().Disconnect(this, &GameState::onKeyDown);
	e.mouseButtonDown().Disconnect(this, &GameState::onMouseDown);
	e.mouseButtonUp().Disconnect(this, &GameState::onMouseUp);
	e.mouseMotion().Disconnect(this, &GameState::onMouseMove);

}


/**
 * Initialize values, the UI and set up event handling.
 */
void GameState::initialize()
{
	mReturnState = this;

	EventHandler& e = Utility<EventHandler>::get();

	e.activate().Connect(this, &GameState::onActivate);

	e.keyDown().Connect(this, &GameState::onKeyDown);

	e.mouseButtonDown().Connect(this, &GameState::onMouseDown);
	e.mouseButtonUp().Connect(this, &GameState::onMouseUp);
	e.mouseMotion().Connect(this, &GameState::onMouseMove);

	// UI
	initUi();

	MENU_ICON(Utility<Renderer>::get().width() - constants::MARGIN_TIGHT * 2 - constants::RESOURCE_ICON_SIZE, 0, constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2, constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2);

	mPointers.push_back(Pointer("ui/pointers/normal.png", 0, 0));
	mPointers.push_back(Pointer("ui/pointers/place_tile.png", 16, 16));
	mPointers.push_back(Pointer("ui/pointers/inspect.png", 8, 8));

	mPlayerResources.capacity(constants::BASE_STORAGE_CAPACITY);

	Utility<Renderer>::get().fadeIn(constants::FADE_SPEED);
}


/**
 * Updates the entire state of the game.
 */
State* GameState::update()
{
	Renderer& r = Utility<Renderer>::get();

	//r.drawImageStretched(mBackground, 0, 0, r.width(), r.height());
	r.drawBoxFilled(0, 0, r.width(), r.height(), 25, 25, 25);

	mTileMap.injectMouse(mMousePosition.x(), mMousePosition.y());
	mTileMap.draw();
	drawUI();

	if(mDebug) drawDebug();

	if (r.isFading())
		return this;

	return mReturnState;
}


void GameState::drawMiniMap()
{
	Renderer& r = Utility<Renderer>::get();

	if(mBtnToggleHeightmap.toggled())
		r.drawImage(mHeightMap, mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y());
	else
		r.drawImage(mMapDisplay, mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y());

	r.drawBox(mMiniMapBoundingBox, 0, 0, 0);

	if (mCCLocation.x() != 0 && mCCLocation.y() != 0)
	{
		r.drawBoxFilled(mCCLocation.x() + mMiniMapBoundingBox.x() - 1, mCCLocation.y() + mMiniMapBoundingBox.y() - 1, 3, 3, 255, 255, 255);
		//r.drawCircle(mCCLocation.x() + mMiniMapBoundingBox.x(), mCCLocation.y() + mMiniMapBoundingBox.y(), 15, 255, 255, 0, 255, 16);
	}


	for(size_t i = 0; i < mTileMap.mineLocations().size(); i++)
	{
		if (mTileMap.getTile(mTileMap.mineLocations()[i].x(), mTileMap.mineLocations()[i].y(), 0)->mine()->active())
			r.drawSubImage(mUiIcons, mTileMap.mineLocations()[i].x() + mMiniMapBoundingBox.x() - 2, mTileMap.mineLocations()[i].y() + mMiniMapBoundingBox.y() - 2, 8.0f, 0.0f, 5.0f, 5.0f);
		else
			r.drawSubImage(mUiIcons, mTileMap.mineLocations()[i].x() + mMiniMapBoundingBox.x() - 2, mTileMap.mineLocations()[i].y() + mMiniMapBoundingBox.y() - 2, 0.0f, 0.0f, 5.0f, 5.0f);
	}

	r.drawBox(mMiniMapBoundingBox.x() + mTileMap.mapViewLocation().x() + 1, mMiniMapBoundingBox.y() + mTileMap.mapViewLocation().y() + 1, mTileMap.edgeLength(), mTileMap.edgeLength(), 0, 0, 0, 180);
	r.drawBox(mMiniMapBoundingBox.x() + mTileMap.mapViewLocation().x(), mMiniMapBoundingBox.y() + mTileMap.mapViewLocation().y(), mTileMap.edgeLength(), mTileMap.edgeLength(), 255, 255, 255);
}


int GameState::foodInStorage()
{
	int food_count = 0;

	auto sl = mStructureManager.structureList(Structure::STRUCTURE_FOOD_PRODUCTION);
	for (size_t i = 0; i < sl.size(); ++i)
	{
		if (sl[i]->operational() || sl[i]->isIdle())
			food_count += sl[i]->storage().food();
	}

	return food_count;
}


int GameState::foodTotalStorage()
{
	int food_storage = 0;

	auto sl = mStructureManager.structureList(Structure::STRUCTURE_FOOD_PRODUCTION);
	for (size_t i = 0; i < sl.size(); ++i)
	{
		if (sl[i]->operational() || sl[i]->isIdle())
			food_storage += sl[i]->storage().capacity();
	}

	return food_storage;
}


void GameState::drawResourceInfo()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(0, 0, r.width(), constants::RESOURCE_ICON_SIZE + 4, 0, 0, 0);
	//r.drawBox(mResourceInfoBox, 0, 0, 0);

	// Resources
	int x = constants::MARGIN_TIGHT;
	int y = constants::MARGIN_TIGHT;

	int textY = 5;
	int offsetX = constants::RESOURCE_ICON_SIZE + 40;

	// Refined Resources
	r.drawSubImage(mUiIcons, x, y , 64, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.commonMetals(), x + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, x + offsetX, y, 80, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.rareMetals(), (x + offsetX) + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, (x + offsetX) * 2, y, 96, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.commonMinerals(), (x + offsetX) * 2 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, (x + offsetX) * 3, y, 112, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.rareMinerals(), (x + offsetX) * 3 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	// Storage Capacity
	r.drawSubImage(mUiIcons, (x + offsetX) * 5, y, 96, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	
	str_scratch.str("");
	str_scratch << mPlayerResources.currentLevel() << " / " << mPlayerResources.capacity();
	r.drawText(mTinyFont, str_scratch.str(), (x + offsetX) * 5 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);


	// Food & Energy
	r.drawSubImage(mUiIcons, (x + offsetX) * 7, y, 64, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	str_scratch.str("");
	str_scratch << foodInStorage() << " / " << foodTotalStorage();
	r.drawText(mTinyFont, str_scratch.str(), (x + offsetX) * 7 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);
	
	
	r.drawSubImage(mUiIcons, (x + offsetX) * 8, y, 80, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	str_scratch.str("");
	str_scratch << mPlayerResources.energy() << " / " << mStructureManager.totalEnergyProduction();
	r.drawText(mTinyFont, str_scratch.str(), (x + offsetX) * 8 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);
	
	//drawNumber(r, mTinyFont, mPlayerResources.energy(), (x + offsetX) * 8 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	// Turns
	r.drawSubImage(mUiIcons, r.width() - 90, y, 128, 0, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mTurnCount, r.width() - 72, textY, 255, 255, 255);

	// ugly
	if (isPointInRect(mMousePosition, MENU_ICON))
		r.drawSubImage(mUiIcons, MENU_ICON.x() + constants::MARGIN_TIGHT, MENU_ICON.y() + constants::MARGIN_TIGHT, 144, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	else
		r.drawSubImage(mUiIcons, MENU_ICON.x() + constants::MARGIN_TIGHT, MENU_ICON.y() + constants::MARGIN_TIGHT, 128, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);


}


void GameState::drawDebug()
{
	Renderer& r = Utility<Renderer>::get();

	stringstream str;
	str << "FPS: " << mFps.fps();
	r.drawText(mFont, str.str(), 10, 25, 255, 255, 255);

	str.str("");
	str << "Map Dimensions: " << mTileMap.width() << ", " << mTileMap.height();
	r.drawText(mFont, str.str(), 10, 25 + mFont.height(), 255, 255, 255);

	str.str("");
	str << "Max Digging Depth: " << mTileMap.maxDepth();
	r.drawText(mFont, str.str(), 10, 25 + mFont.height() * 2, 255, 255, 255);


	str.str("");
	str << "Structure Count: " << mStructureManager.count();
	r.drawText(mFont, str.str(), 10, 25 + mFont.height() * 4, 255, 255, 255);
}


/**
 * Window activation handler.
 */
void GameState::onActivate(bool _b)
{
	if (!_b)
	{
		mLeftButtonDown = false;
	}
}


/**
 * Key down event handler.
 */
void GameState::onKeyDown(KeyCode key, KeyModifier mod, bool repeat)
{
	Point_2d pt = mTileMap.mapViewLocation();

	switch(key)
	{
		case KEY_w:
		case KEY_UP:
			pt.y(clamp(--pt.y(), 0, mTileMap.height() - mTileMap.edgeLength()));
			break;

		case KEY_s:
		case KEY_DOWN:
			pt.y(clamp(++pt.y(), 0, mTileMap.height() - mTileMap.edgeLength()));
			break;

		case KEY_a:
		case KEY_LEFT:
			pt.x(clamp(--pt.x(), 0, mTileMap.width() - mTileMap.edgeLength()));
			break;

		case KEY_d:
		case KEY_RIGHT:
			pt.x(clamp(++pt.x(), 0, mTileMap.width() - mTileMap.edgeLength()));
			break;

		case KEY_0:
			changeDepth(0);
			break;

		case KEY_1:
			changeDepth(1);
			break;

		case KEY_2:
			changeDepth(2);
			break;

		case KEY_3:
			changeDepth(3);
			break;

		case KEY_4:
			changeDepth(4);
			break;

		case KEY_F10:
			mDebug = !mDebug;
			break;

		case KEY_F2:
			save(constants::SAVE_GAME_PATH + "test.xml");
			break;

		case KEY_F3:
			//load("test.xml");
			break;
		case KEY_ESCAPE:
			clearMode();
			resetUi();
			break;

		default:
			break;
	}

	mTileMap.mapViewLocation(pt.x(), pt.y());
}


void GameState::changeDepth(int _d)
{
	mTileMap.currentDepth(_d);
	clearMode();
	populateStructureMenu();
}


void GameState::clearMode()
{
	mInsertMode = INSERT_NONE;
	mCurrentPointer = POINTER_NORMAL;

	mCurrentStructure = SID_NONE;
	mCurrentRobot = ROBOT_NONE;

	clearSelections();
}


/**
 * Mouse Down event handler.
 */
void GameState::onMouseDown(MouseButton button, int x, int y)
{
	// Cludgy but basically if this dialog is open, fuck everything else.
	if (mFactoryProduction.visible())
		return;

	if(button == BUTTON_RIGHT)
	{
		Tile* _t = mTileMap.getTile(mTileMap.tileHighlight().x() + mTileMap.mapViewLocation().x(), mTileMap.tileHighlight().y() + mTileMap.mapViewLocation().y());

		if(mInsertMode != INSERT_NONE)
		{
			clearMode();
			return;
		}

		if (!mTileMap.tileHighlightVisible())
			return;

		if (!_t)
		{
			return;
		}
		else if(_t->empty() && isPointInRect(mMousePosition, mTileMap.boundingBox()))
		{
			clearSelections();
			mTileInspector.tile(_t);
			mTileInspector.visible(true); 
		}
		else if (_t->structure() && _t->structure()->isFactory())
		{
			if (_t->structure()->state() == Structure::OPERATIONAL || _t->structure()->state() == Structure::IDLE)
			{
				mFactoryProduction.factory(static_cast<Factory*>(_t->structure()));
				mFactoryProduction.show();
			}
		}
	}

	if(button == BUTTON_LEFT)
	{
		mLeftButtonDown = true;

		// If mouse pointer is within the rects of a a UI element, ignore it.
		if (mDiggerDirection.visible() && isPointInRect(mMousePosition, mDiggerDirection.rect()))
			return;

		// Ugly
		if (isPointInRect(mMousePosition, MENU_ICON))
		{
			mReturnState = new PlanetSelectState();
			Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
			return;
		}
		
		// MiniMap Check
		if(isPointInRect(mMousePosition, mMiniMapBoundingBox))
		{
			updateMapView();
		}
		// Click was within the bounds of the TileMap.
		else if(isPointInRect(mMousePosition, mTileMap.boundingBox()))
		{
			if(mInsertMode == INSERT_STRUCTURE)
			{
				placeStructure();
			}
			else if(mInsertMode == INSERT_ROBOT)
			{
				placeRobot();
			}
			else if(mInsertMode == INSERT_TUBE)
			{
				placeTubes();
			}
		}
	}
}


void GameState::placeTubes()
{
	int x = mTileMapMouseHover.x();
	int y = mTileMapMouseHover.y();

	Tile* tile = mTileMap.getTile(x, y, mTileMap.currentDepth());
	if(!tile)
		return;

	// Check the basics.
	if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated())
		return;

	if (validTubeConnection(x, y, mCurrentStructure))
	{
		// FIXME:	This can be done a lot better.
		if(mCurrentStructure == SID_TUBE_INTERSECTION)
			mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, mTileMap.currentDepth() != 0), mTileMap.getTile(x, y));
		else if (mCurrentStructure == SID_TUBE_RIGHT)
			mStructureManager.addStructure(new Tube(CONNECTOR_RIGHT, mTileMap.currentDepth() != 0), mTileMap.getTile(x, y));
		else if (mCurrentStructure == SID_TUBE_LEFT)
			mStructureManager.addStructure(new Tube(CONNECTOR_LEFT, mTileMap.currentDepth() != 0), mTileMap.getTile(x, y));
		else
			throw Exception(0, "Structure Not a Tube", "GameState::placeTube() called but Current Structure is not a tube!");

		// FIXME:	Naive approach. This will be slow with larger colonies,
		//			especially colonies that have expanded far underground.
		mStructureManager.disconnectAll();
		checkConnectedness();
	}
}


/**
 * Checks to see if a tile is a valid tile to place a tube onto.
 */
bool GameState::validTubeConnection(int x, int y, StructureID type)
{

	return	checkTubeConnection(mTileMap.getTile(x + 1, y, mTileMap.currentDepth()), DIR_EAST, type) ||
			checkTubeConnection(mTileMap.getTile(x - 1, y, mTileMap.currentDepth()), DIR_WEST, type) ||
			checkTubeConnection(mTileMap.getTile(x, y + 1, mTileMap.currentDepth()), DIR_SOUTH, type) ||
			checkTubeConnection(mTileMap.getTile(x, y - 1, mTileMap.currentDepth()), DIR_NORTH, type);
}


/**
 * Checks a tile to see if a valid Tube connection is available for Structure placement.
 */
bool GameState::validStructurePlacement(int x, int y)
{

	return	checkStructurePlacement(mTileMap.getTile(x, y - 1), DIR_NORTH) ||
			checkStructurePlacement(mTileMap.getTile(x + 1, y), DIR_EAST) ||
			checkStructurePlacement(mTileMap.getTile(x, y + 1), DIR_SOUTH) ||
			checkStructurePlacement(mTileMap.getTile(x - 1, y), DIR_WEST);
}


void GameState::placeRobot()
{
	Tile* tile = mTileMap.getTile(mTileMapMouseHover.x(), mTileMapMouseHover.y(), mTileMap.currentDepth());
	if(!tile)
		return;

	// Robodozer has been selected.
	if(mCurrentRobot == ROBOT_DOZER)
	{
		if(tile->mine() || !tile->excavated() || tile->thing() && !tile->thingIsStructure())
			return;
		else if (tile->thingIsStructure())
		{
			Structure* _s = tile->structure();
			if (_s->name() == constants::COMMAND_CENTER)
			{
				cout << "Can't bulldoze a Command Center!" << endl;
				return;
			}

			mPlayerResources.pushResources(_s->resourcesRecyclingValue());

			tile->connected(false);
			mStructureManager.removeStructure(_s);
			tile->deleteThing();
			mStructureManager.disconnectAll();
			checkConnectedness();
		}
		else if (tile->index() == TERRAIN_DOZED)
			return;

		Robot* r = mRobotPool.getDozer();
		r->startTask(tile->index());
		insertRobot(r, tile);
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
		if (mTileMapMouseHover.x() < 3 || mTileMapMouseHover.x() > mTileMap.width() - 4 || mTileMapMouseHover.y() < 3 || mTileMapMouseHover.y() > mTileMap.height() - 4)
		{
			cout << "GameState::placeRobot(): Can't place digger within 3 tiles of the edge of a map." << endl;
			return;
		}

		// Die if tile is occupied or not excavated.
		if (!tile->empty() && tile->structure() != nullptr && tile->structure()->connectorDirection() != CONNECTOR_VERTICAL)
			return;
		else if (tile->mine() || !tile->excavated())
			return;
		else if (!tile->thing() && mTileMap.currentDepth() > 0)
			mDiggerDirection.cardinalOnlyEnabled();
		else
			mDiggerDirection.downOnlyEnabled();

		//hideUi();
		mDiggerDirection.setParameters(tile);

		// NOTE:	Unlike the Dozer and Miner, Digger's aren't removed here but instead
		//			are removed after responses to the DiggerDirection dialog.

		// If we're placing on the top level we can only ever go down.
		if (mTileMap.currentDepth() == 0)
			mDiggerDirection.selectDown();
		else
			mDiggerDirection.visible(true);
	}
	// Robominer has been selected.
	else if(mCurrentRobot == ROBOT_MINER)
	{
		if(tile->thing() || !tile->mine() || !tile->excavated())
			return;

		Robot* r = mRobotPool.getMiner();
		r->startTask(6);
		insertRobot(r, tile);
		tile->index(TERRAIN_DOZED);

		clearMode();

		if(!mRobotPool.robotAvailable(ROBOT_MINER))
			mRobots.removeItem(constants::ROBOMINER);
	}

	if (mRobotPool.allRobotsBusy())
	{
		mBtnRobots.enabled(false);
		//mBtnRobots.toggle(false);
		//mRobots.hide();
	}
}


/** 
 * Checks the robot selection interface and if the robot is not available in it, adds
 * it back in and reeneables the robots button if it's not enabled.
 */
void GameState::checkRobotSelectionInterface(const std::string rType, int sheetIndex)
{
	if (!mRobots.itemExists(rType))
	{
		mRobots.addItem(rType, sheetIndex);

		if (!mBtnRobots.enabled())
			mBtnRobots.enabled(true);
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
	if(mRobotList.find(_r) == mRobotList.end())
		throw Exception(0, "Renegade Robot", "GameState::diggerTaskFinished() called with a Robot not in the Robot List!");

	Tile* t = mRobotList[_r];

	if (t->depth() > mTileMap.maxDepth())
		throw Exception(0, "Bad Depth", "Digger defines a depth that exceeds the maximum digging depth!");

	// FIXME: Fugly cast.
	Direction dir = static_cast<Robodigger*>(_r)->direction();

	// 
	int originX = 0, originY = 0, depthAdjust = 0;

	if(dir == DIR_DOWN)
	{
		AirShaft* as1 = new AirShaft();
		if (t->depth() > 0) as1->ug();
		mStructureManager.addStructure(as1, t);

		AirShaft* as2 = new AirShaft();
		as2->ug();
		mStructureManager.addStructure(as2, mTileMap.getTile(t->x(), t->y(), t->depth() + 1));

		originX = t->x();
		originY = t->y();
		depthAdjust = 1;

		mTileMap.getTile(originX, originY, t->depth())->index(TERRAIN_DOZED);
		mTileMap.getTile(originX, originY, t->depth() + depthAdjust)->index(TERRAIN_DOZED);

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
			mTileMap.getTile(x, y, t->depth() + depthAdjust)->excavated(true);
		}
	}

	checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID);
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void GameState::minerTaskFinished(Robot* _r)
{
	if(mRobotList.find(_r) == mRobotList.end())
		throw Exception(0, "Renegade Robot", "GameState::minerTaskFinished() called with a Robot not in the Robot List!");

	Tile* t = mRobotList[_r];

	mStructureManager.addStructure(new MineFacility(t->mine()), t);

	checkRobotSelectionInterface(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID);
}


/**
 * Called whenever a Factory's production is complete.
 */
void GameState::factoryProductionComplete(Factory::ProductionType _p)
{
	switch (_p)
	{
	case Factory::PRODUCTION_DIGGER:
		mRobotPool.addRobot(ROBOT_DIGGER)->taskComplete().Connect(this, &GameState::diggerTaskFinished);
		break;
	case Factory::PRODUCTION_DOZER:
		mRobotPool.addRobot(ROBOT_DOZER)->taskComplete().Connect(this, &GameState::dozerTaskFinished);
		break;
	case Factory::PRODUCTION_MINER:
		mRobotPool.addRobot(ROBOT_MINER)->taskComplete().Connect(this, &GameState::minerTaskFinished);
		break;
	default:
		break;
	}
}

/**
 * Places a structure into the map.
 */
void GameState::placeStructure()
{
	if (mCurrentStructure == SID_NONE)
	{
		#ifdef _DEBUG
		throw Exception(0, "Invalid call to placeStructure()", "GameState::placeStructure() called but mCurrentStructure == STRUCTURE_NONE");
		#endif

		// When not in Debug just silently swallow this.
		return;
	}

	// Mouse is outside of the boundaries of the map so ignore this call.
	Tile* t = mTileMap.getTile(mTileMapMouseHover.x(), mTileMapMouseHover.y(), mTileMap.currentDepth());
	if(!t)
		return;

	if(t->mine() || t->thing() || !t->bulldozed() && mCurrentStructure != SID_SEED_LANDER)
	{
		// TODO: Make this issue obvious to the user in the game's UI so there is no
		// confusion as to why the structure wasn't placed.
		cout << "GameState::placeStructure(): Tile is unsuitable to place a structure." << endl;
		return;
	}

	// Seed lander is a special case and only one can ever be placed by the player ever.
	if(mCurrentStructure == SID_SEED_LANDER)
	{
		insertSeedLander(mTileMapMouseHover.x(), mTileMapMouseHover.y());
	}
	else
	{
		// Check build cost
		ResourcePool rp = StructureFactory::costToBuild(mCurrentStructure);
		if (mPlayerResources.commonMetals() < rp.commonMetals() || mPlayerResources.commonMinerals() < rp.commonMinerals() ||
			mPlayerResources.rareMetals() < rp.rareMetals() || mPlayerResources.rareMinerals() < rp.rareMinerals())
		{
			// TODO: Make this issue obvious to the user in the game's UI so there is no
			// confusion as to why the structure wasn't placed.
			cout << "GameState::placeStructure(): Insufficient resources to build structure." << endl;
			return;
		}

		if (!validStructurePlacement(mTileMapMouseHover.x(), mTileMapMouseHover.y()))
		{
			// TODO: Make this issue obvious to the user in the game's UI so there is no
			// confusion as to why the structure wasn't placed.
			cout << "GameState::placeStructure(): Invalid structure placement." << endl;
			return;
		}

		Structure* _s = StructureFactory::get(mCurrentStructure);
		if (_s)
			mStructureManager.addStructure(_s, t);

		// FIXME: Ugly
		if (_s->isFactory())
			static_cast<Factory*>(_s)->productionComplete().Connect(this, &GameState::factoryProductionComplete);

		mPlayerResources -= rp;
	}
}


/**
 * Mouse Up event handler.
 */
void GameState::onMouseUp(MouseButton button, int x, int y)
{
	if(button == BUTTON_LEFT)
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
	mTileMapMouseHover(mTileMap.tileMouseHoverX(), mTileMap.tileMouseHoverY());

	if(mLeftButtonDown)
	{
		if(isPointInRect(mMousePosition, mMiniMapBoundingBox))
		{
			updateMapView();
		}
	}
}

void GameState::updateMapView()
{
	int x = clamp(mMousePosition.x() - mMiniMapBoundingBox.x() - mTileMap.edgeLength() / 2, 0, mTileMap.width() - mTileMap.edgeLength());
	int y = clamp(mMousePosition.y() - mMiniMapBoundingBox.y() - mTileMap.edgeLength() / 2, 0, mTileMap.height() - mTileMap.edgeLength());

	mTileMap.mapViewLocation(x, y);
}


bool GameState::insertRobot(Robot* robot, Tile* tile)
{
	if(!tile)
		return false;

	RobotMap::iterator it = mRobotList.find(robot);
	if(it != mRobotList.end())
		throw Exception(0, "Duplicate Robot", "GameState::insertRobot(): Attempting to add a duplicate Robot* pointer.");

	mRobotList[robot] = tile;
	tile->pushThing(robot);

	return true;
}


/**
 * Checks that the clicked tile is a suitable spot for the SEED Lander and
 * then inserts it into the the TileMap.
 */
void GameState::insertSeedLander(int x, int y)
{
	// Has to be built away from the edges of the map
	if (x > 3 && x < mTileMap.width() - 4 && y > 3 && y < mTileMap.height() - 4)
	{
		// check for obstructions
		if (!landingSiteSuitable(x, y))
		{
			cout << "Unable to place SEED Lander. Tiles obstructed." << endl;
			return;
		}

		SeedLander* s = new SeedLander(x, y);
		s->deployCallback().Connect(this, &GameState::deploySeedLander);
		mStructureManager.addStructure(s, mTileMap.getTile(x, y)); // Can only ever be placed on depth level 0

		clearMode();
		resetUi();

		mBtnStructures.enabled(false);
		mStructures.dropAllItems();
		mBtnTurns.enabled(true);
	}
}


/**
 * Check landing site for obstructions such as mining beacons, things
 * and impassable terrain.
 */
bool GameState::landingSiteSuitable(int x, int y)
{
	for(int offY = y - 1; offY <= y + 1; ++offY)
		for(int offX = x - 1; offX <= x + 1; ++offX)
			if(mTileMap.getTile(offX, offY)->index() > TERRAIN_DIFFICULT || mTileMap.getTile(offX, offY)->mine() || mTileMap.getTile(offX, offY)->thing())
				return false;

	return true;
}


/**
 * Sets up the initial colony deployment.
 */
void GameState::deploySeedLander(int x, int y)
{
	mTileMap.getTile(x, y)->index(TERRAIN_DOZED);
	
	// TOP ROW
	mStructureManager.addStructure(new SeedPower(), mTileMap.getTile(x - 1, y - 1));
	mTileMap.getTile(x - 1, y - 1)->index(TERRAIN_DOZED);

	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x, y - 1));
	mTileMap.getTile(x, y - 1)->index(TERRAIN_DOZED);

	CommandCenter* cc = new CommandCenter();
	cc->sprite().skip(3);
	mStructureManager.addStructure(cc, mTileMap.getTile(x + 1, y - 1));
	mTileMap.getTile(x + 1, y - 1)->index(TERRAIN_DOZED);
	mCCLocation(x + 1, y - 1);


	// MIDDLE ROW
	mTileMap.getTile(x - 1, y)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x - 1, y));

	mTileMap.getTile(x + 1, y)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x + 1, y));


	// BOTTOM ROW
	SeedFactory* sf = new SeedFactory();
	sf->resourcePool(&mPlayerResources);
	sf->productionComplete().Connect(this, &GameState::factoryProductionComplete);
	sf->sprite().skip(7);
	mStructureManager.addStructure(sf, mTileMap.getTile(x - 1, y + 1));
	mTileMap.getTile(x - 1, y + 1)->index(TERRAIN_DOZED);

	mTileMap.getTile(x, y + 1)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x, y + 1));

	SeedSmelter* ss = new SeedSmelter();
	ss->sprite().skip(10);
	mStructureManager.addStructure(ss, mTileMap.getTile(x + 1, y + 1));
	mTileMap.getTile(x + 1, y + 1)->index(TERRAIN_DOZED);

	// Enable UI Contruction Buttons
	mBtnConnections.enabled(true);
	mBtnRobots.enabled(true);

	// Robots only become available after the SEED Factor is deployed.
	mRobots.addItem(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID);
	mRobots.addItem(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID);
	mRobots.addItem(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID);

	mRobotPool.addRobot(ROBOT_DOZER)->taskComplete().Connect(this, &GameState::dozerTaskFinished);
	mRobotPool.addRobot(ROBOT_DIGGER)->taskComplete().Connect(this, &GameState::diggerTaskFinished);
	mRobotPool.addRobot(ROBOT_MINER)->taskComplete().Connect(this, &GameState::minerTaskFinished);

	// FIXME: Magic numbers
	mPlayerResources.commonMetals(50);
	mPlayerResources.commonMinerals(50);
	mPlayerResources.rareMetals(30);
	mPlayerResources.rareMinerals(30);

	//mPopulationPool.addWorkers(30);
	//mPopulationPool.addScientists(20);
}



/**
 * Updates all robots.
 */
void GameState::updateRobots()
{
	RobotMap::iterator robot_it = mRobotList.begin();
	while(robot_it != mRobotList.end())
	{
		robot_it->first->update();

		// Clear Idle robots from tiles.
		if(robot_it->first->idle())
		{
			// Make sure that we're the robot from a Tile and not something else
			if(robot_it->second->thing() == robot_it->first)
				robot_it->second->removeThing();
	
			robot_it = mRobotList.erase(robot_it);
		}
		else
			++robot_it;
	}
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
	mCurrentPointer = POINTER_PLACE_TILE;
}


/**
 * Checks the connectedness of all tiles surrounding
 * the Command Center.
 */
void GameState::checkConnectedness()
{
	if (mCCLocation.x() == 0 && mCCLocation.y() == 0)
		return;

	// Assumes that the 'thing' at mCCLocation is in fact a Structure.
	Tile *t = mTileMap.getTile(mCCLocation.x(), mCCLocation.y(), 0);
	Structure *cc = t->structure();

	if (!cc)
		throw Exception(0, "Bad CC Location", "CC coordinates do not actually point to a Command Center.");

	if (cc->state() == Structure::UNDER_CONSTRUCTION)
		return;

	t->connected(true);

	// Start graph walking at the CC location.
	GraphWalker graphWalker(mCCLocation, 0, &mTileMap);
}


// Convenience functions
void checkRobotDeployment(TiXmlElement* _ti, GameState::RobotMap& _rm, Robot* _r, RobotType _type);
void writeRobots(TiXmlElement* _ti, RobotPool& _rp, GameState::RobotMap& _rm);
void writeResources(TiXmlElement* _ti, ResourcePool& _rp);


void GameState::save(const std::string& _path)
{
	TiXmlDocument doc;

	//TiXmlComment *comment = new TiXmlComment("Automatically generated Configuration file. This is best left untouched.");
	//doc->LinkEndChild(comment);

	TiXmlElement* root = new TiXmlElement("OutpostHD_SaveGame");
	root->SetAttribute("version", constants::SAVE_GAME_VERSION);
	doc.LinkEndChild(root);

	mTileMap.serialize(root);
	mStructureManager.serialize(root);
	writeRobots(root, mRobotPool, mRobotList);
	writeResources(root, mPlayerResources);

	TiXmlElement* turns = new TiXmlElement("turns");
	turns->SetAttribute("count", mTurnCount);
	root->LinkEndChild(turns);

	// Write out the XML file.
	TiXmlPrinter printer;
	doc.Accept(&printer);

	Utility<Filesystem>::get().write(File(printer.Str(), _path));
}




// ==============================================================
// = CONVENIENCE FUNCTIONS FOR WRITING OUT GAME STATE INFORMATION
// ==============================================================
void checkRobotDeployment(TiXmlElement* _ti, GameState::RobotMap& _rm, Robot* _r, RobotType _type)
{
	_ti->SetAttribute("type", _type);
	_ti->SetAttribute("age", _r->fuelCellAge());
	_ti->SetAttribute("production", _r->turnsToCompleteTask());
	_ti->SetAttribute("deployed", false);

	for (auto it = _rm.begin(); it != _rm.end(); ++it)
	{
		if (it->first == _r)
		{
			_ti->SetAttribute("deployed", true);
			_ti->SetAttribute("x", it->second->x());
			_ti->SetAttribute("y", it->second->y());
			_ti->SetAttribute("depth", it->second->depth());
		}
	}

}

// Convenience function
void writeRobots(TiXmlElement* _ti, RobotPool& _rp, GameState::RobotMap& _rm)
{
	TiXmlElement* robots = new TiXmlElement("robots");

	RobotPool::DiggerList& diggers = _rp.diggers();
	for (size_t i = 0; i < diggers.size(); ++i)
	{
		TiXmlElement* robot = new TiXmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(diggers[i]), ROBOT_DIGGER);
		robot->SetAttribute("direction", diggers[i]->direction());
		robots->LinkEndChild(robot);
	}

	RobotPool::DozerList& dozers = _rp.dozers();
	for (size_t i = 0; i < dozers.size(); ++i)
	{
		TiXmlElement* robot = new TiXmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(dozers[i]), ROBOT_DOZER);
		robots->LinkEndChild(robot);
	}

	RobotPool::MinerList& miners = _rp.miners();
	for (size_t i = 0; i < miners.size(); ++i)
	{
		TiXmlElement* robot = new TiXmlElement("robot");
		checkRobotDeployment(robot, _rm, static_cast<Robot*>(miners[i]), ROBOT_MINER);
		robots->LinkEndChild(robot);
	}

	_ti->LinkEndChild(robots);
}


void writeResources(TiXmlElement* _ti, ResourcePool& _rp)
{
	TiXmlElement* resources = new TiXmlElement("resources");
	_rp.serialize(resources);
	_ti->LinkEndChild(resources);
}