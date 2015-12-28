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

/**
 * C'Tor
 */
GameState::GameState(const string& map, const string& tset):	mFont("fonts/Fresca-Regular.ttf", 14),
																mTinyFont("fonts/Fresca-Regular.ttf", 10),
																mTileMap(map, tset, MAX_DEPTH),
																mBackground("ui/background.png"),
																mMapDisplay(map + MAP_DISPLAY_EXTENSION),
																mHeightMap(map + MAP_TERRAIN_EXTENSION),
																mUiIcons("ui/icons.png"),
																mCurrentPointer(POINTER_NORMAL),
																mCurrentStructure(STRUCTURE_NONE),
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
		it->second.tile->removeThing();
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

	if(mCCLocation.x() != 0 && mCCLocation.y() != 0)
		r.drawBoxFilled(mCCLocation.x() + mMiniMapBoundingBox.x() - 1, mCCLocation.y() + mMiniMapBoundingBox.y() - 1, 3, 3, 255, 255, 255);

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


void GameState::drawResourceInfo()
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(0, 0, r.width(), constants::RESOURCE_ICON_SIZE + 4, 0, 0, 0);
	//r.drawBox(mResourceInfoBox, 0, 0, 0);

	// Resources
	int x = constants::MARGIN_TIGHT;
	int y = constants::MARGIN_TIGHT;

	int textY = 5;
	int offsetX = constants::RESOURCE_ICON_SIZE + 45;

	// Refined Resources
	r.drawSubImage(mUiIcons, x, y , 64, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.commonMetals, x + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, x + offsetX, y, 80, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.rareMetals, (x + offsetX) + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, (x + offsetX) * 2, y, 96, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.commonMinerals, (x + offsetX) * 2 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, (x + offsetX) * 3, y, 112, 16, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.rareMinerals, (x + offsetX) * 3 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	// Food & Energy
	r.drawSubImage(mUiIcons, (x + offsetX) * 5, y, 64, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.food, (x + offsetX) * 5 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

	r.drawSubImage(mUiIcons, (x + offsetX) * 6, y, 80, 32, constants::RESOURCE_ICON_SIZE, constants::RESOURCE_ICON_SIZE);
	drawNumber(r, mTinyFont, mPlayerResources.energy, (x + offsetX) * 6 + (constants::RESOURCE_ICON_SIZE + constants::MARGIN), textY, 255, 255, 255);

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
	r.drawText(mTinyFont, str.str(), 10, 10, 255, 255, 255);
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

		case KEY_F1:
			mStructureManager.printSortedList();
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

	mCurrentStructure = STRUCTURE_NONE;
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
		if (isPointInRect(mMousePosition, mDiggerDirection.rect()))
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
		if(mCurrentStructure == STRUCTURE_TUBE_INTERSECTION)
			mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, mTileMap.currentDepth() != 0), mTileMap.getTile(x, y), x, y, mTileMap.currentDepth(), true);
		else if (mCurrentStructure == STRUCTURE_TUBE_RIGHT)
			mStructureManager.addStructure(new Tube(CONNECTOR_RIGHT, mTileMap.currentDepth() != 0), mTileMap.getTile(x, y), x, y, mTileMap.currentDepth(), true);
		else if (mCurrentStructure == STRUCTURE_TUBE_LEFT)
			mStructureManager.addStructure(new Tube(CONNECTOR_LEFT, mTileMap.currentDepth() != 0), mTileMap.getTile(x, y), x, y, mTileMap.currentDepth(), true);
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
bool GameState::validTubeConnection(int x, int y, StructureType type)
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

			mPlayerResources += _s->resourceValue();

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
		insertRobot(r, tile, mTileMapMouseHover.x(), mTileMapMouseHover.y(), mTileMap.currentDepth());
		tile->index(TERRAIN_DOZED);

		if(!mRobotPool.robotAvailable(RobotPool::ROBO_DOZER))
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
		mDiggerDirection.setParameters(tile, mTileMapMouseHover.x(), mTileMapMouseHover.y(), mTileMap.currentDepth());

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
		insertRobot(r, tile, mTileMapMouseHover.x(), mTileMapMouseHover.y(), mTileMap.currentDepth());
		tile->index(TERRAIN_DOZED);

		clearMode();

		if(!mRobotPool.robotAvailable(RobotPool::ROBO_MINER))
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
 * Called whenever a RoboDozer completes its task.
 */
void GameState::dozerTaskFinished(Robot* _r)
{
	if(!mRobots.itemExists(constants::ROBODOZER))
	{
		mRobots.addItem(constants::ROBODOZER, 0);

		if(!mBtnRobots.enabled())
			mBtnRobots.enabled(true);
	}
}


/**
 * Called whenever a RoboDigger completes its task.
 */
void GameState::diggerTaskFinished(Robot* _r)
{
	if(mRobotList.find(_r) == mRobotList.end())
		throw Exception(0, "Renegade Robot", "GameState::diggerTaskFinished() called with a Robot not in the Robot List!");

	TilePositionInfo tpi = mRobotList[_r];

	if (tpi.depth > mTileMap.maxDepth())
		throw Exception(0, "Bad Depth", "Digger defines a depth that exceeds the maximum digging depth!");

	// FIXME: Fugly cast.
	Direction dir = static_cast<Robodigger*>(_r)->direction();

	// 
	int originX = 0, originY = 0, depthAdjust = 0;

	if(dir == DIR_DOWN)
	{
		AirShaft* as1 = new AirShaft();
		if (tpi.depth > 0) as1->ug();
		mStructureManager.addStructure(as1, tpi.tile, tpi.x, tpi.y, tpi.depth, false);

		AirShaft* as2 = new AirShaft();
		as2->ug();
		mStructureManager.addStructure(as2, mTileMap.getTile(tpi.x, tpi.y, tpi.depth + 1), tpi.x, tpi.y, tpi.depth + 1, false);

		originX = tpi.x;
		originY = tpi.y;
		depthAdjust = 1;

		mTileMap.getTile(originX, originY, tpi.depth)->index(TERRAIN_DOZED);
		mTileMap.getTile(originX, originY, tpi.depth + depthAdjust)->index(TERRAIN_DOZED);
	}
	else if(dir == DIR_NORTH)
	{
		originX = tpi.x;
		originY = tpi.y - 1;
	}
	else if(dir == DIR_SOUTH)
	{
		originX = tpi.x;
		originY = tpi.y + 1;
	}
	else if(dir == DIR_WEST)
	{
		originX = tpi.x - 1;
		originY = tpi.y;
	}
	else if(dir == DIR_EAST)
	{
		originX = tpi.x + 1;
		originY = tpi.y;
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
			mTileMap.getTile(x, y, tpi.depth + depthAdjust)->excavated(true);
		}
	}


	if(!mRobots.itemExists(constants::ROBODIGGER))
	{
		mRobots.addItem(constants::ROBODIGGER, 1);

		if(!mBtnRobots.enabled())
			mBtnRobots.enabled(true);
	}
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void GameState::minerTaskFinished(Robot* _r)
{
	if(mRobotList.find(_r) == mRobotList.end())
		throw Exception(0, "Renegade Robot", "GameState::minerTaskFinished() called with a Robot not in the Robot List!");

	TilePositionInfo tpi = mRobotList[_r];

	mStructureManager.addStructure(new MineFacility(tpi.tile->mine()), tpi.tile, tpi.x, tpi.y, tpi.depth, false);

	if(!mRobots.itemExists(constants::ROBOMINER))
	{
		mRobots.addItem(constants::ROBOMINER, 2);

		if(!mBtnRobots.enabled())
			mBtnRobots.enabled(true);
	}
}


/**
 * Places a structure into the map.
 */
void GameState::placeStructure()
{
	if (mCurrentStructure == STRUCTURE_NONE)
		return;

	Tile* tile = mTileMap.getTile(mTileMapMouseHover.x(), mTileMapMouseHover.y(), mTileMap.currentDepth());
	if(!tile)
		return;

	if(tile->mine() || tile->thing() || !tile->bulldozed() && mCurrentStructure != STRUCTURE_SEED_LANDER)
	{
		cout << "GameState::placeStructure(): Tile is unsuitable to place a structure." << endl;
		return;
	}

	// Seed lander is a special case and only one can ever be placed by the player ever.
	if(mCurrentStructure == STRUCTURE_SEED_LANDER)
	{
		insertSeedLander(mTileMapMouseHover.x(), mTileMapMouseHover.y());
	}
	else
	{
		if (!validStructurePlacement(mTileMapMouseHover.x(), mTileMapMouseHover.y()))
		{
			cout << "GameState::placeStructure(): Invalid structure placement." << endl;
			return;
		}

		Structure* _s = StructureFactory::get(mCurrentStructure);
		if (_s)
			mStructureManager.addStructure(_s, tile, mTileMapMouseHover.x(), mTileMapMouseHover.y(), mTileMap.currentDepth(), false);
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


bool GameState::insertRobot(Robot* robot, Tile* tile, int x, int y, int depth)
{
	if(!tile)
		return false;

	RobotMap::iterator it = mRobotList.find(robot);
	if(it != mRobotList.end())
		throw Exception(0, "Duplicate Robot", "GameState::insertRobot(): Attempting to add a duplicate Robot* pointer.");

	mRobotList[robot] = TilePositionInfo(tile, x, y, depth);
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
		mStructureManager.addStructure(s, mTileMap.getTile(x, y), x, y, 0, true); // Can only ever be placed on depth level 0

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
	mStructureManager.addStructure(new SeedPower(), mTileMap.getTile(x - 1, y - 1), x - 1, y - 1, 0, true);
	mTileMap.getTile(x - 1, y - 1)->index(TERRAIN_DOZED);

	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x, y - 1), x, y - 1, 0, true);
	mTileMap.getTile(x, y - 1)->index(TERRAIN_DOZED);

	CommandCenter* cc = new CommandCenter();
	cc->sprite().skip(3);
	mStructureManager.addStructure(cc, mTileMap.getTile(x + 1, y - 1), x + 1, y - 1, 0, true);
	mTileMap.getTile(x + 1, y - 1)->index(TERRAIN_DOZED);
	mCCLocation(x + 1, y - 1);


	// MIDDLE ROW
	mTileMap.getTile(x - 1, y)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x - 1, y), x - 1, y, 0, true);

	mTileMap.getTile(x + 1, y)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x + 1, y), x + 1, y, 0, true);


	// BOTTOM ROW
	SeedFactory* sf = new SeedFactory();
	sf->resourcePool(&mPlayerResources);
	sf->robotPool(&mRobotPool);
	sf->sprite().skip(7);
	mStructureManager.addStructure(sf, mTileMap.getTile(x - 1, y + 1), x - 1, y + 1, 0, true);
	mTileMap.getTile(x - 1, y + 1)->index(TERRAIN_DOZED);

	mTileMap.getTile(x, y + 1)->index(TERRAIN_DOZED);
	mStructureManager.addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap.getTile(x, y + 1), x, y + 1, 0, true);

	SeedSmelter* ss = new SeedSmelter();
	ss->resourcePool(&mPlayerResources);
	ss->sprite().skip(10);
	mStructureManager.addStructure(ss, mTileMap.getTile(x + 1, y + 1), x + 1, y + 1, 0, true);
	mTileMap.getTile(x + 1, y + 1)->index(TERRAIN_DOZED);

	// Enable UI Contruction Buttons
	mBtnConnections.enabled(true);
	mBtnRobots.enabled(true);

	// Robots only become available after the SEED Factor is deployed.
	mRobots.addItem(constants::ROBODOZER, 0);
	mRobots.addItem(constants::ROBODIGGER, 1);
	mRobots.addItem(constants::ROBOMINER, 2);

	mRobotPool.addRobot(RobotPool::ROBO_DOZER)->taskComplete().Connect(this, &GameState::dozerTaskFinished);
	mRobotPool.addRobot(RobotPool::ROBO_DIGGER)->taskComplete().Connect(this, &GameState::diggerTaskFinished);
	mRobotPool.addRobot(RobotPool::ROBO_MINER)->taskComplete().Connect(this, &GameState::minerTaskFinished);

	// FIXME: Magic numbers
	mPlayerResources.commonMetals = 100;
	mPlayerResources.commonMinerals = 85;
	mPlayerResources.rareMetals = 50;
	mPlayerResources.rareMinerals = 60;

	mPopulationPool.addWorkers(30);
	mPopulationPool.addScientists(20);
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

		/**
		 * Clear Idle robots from tiles.
		 */
		if(robot_it->first->idle())
		{
			// Make sure that we're the robot from a Tile and not something else
			if(robot_it->second.tile->thing() == robot_it->first)
				robot_it->second.tile->removeThing();
	
			robot_it = mRobotList.erase(robot_it);
		}
		else
			++robot_it;
	}
}


/**
 * Checks and sets the current structure mode.
 */
void GameState::setStructureType(StructureType type, InsertMode mode)
{

	if (type == STRUCTURE_NONE)
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