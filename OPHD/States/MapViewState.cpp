// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MapViewState.h"
#include "MainMenuState.h"
#include "MainReportsUiState.h"

#include "../Constants.h"
#include "../DirectionOffset.h"
#include "../Cache.h"
#include "../GraphWalker.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"

#include "../Map/Tile.h"
#include "../Map/TileMap.h"

#include "../Things/Robots/Robots.h"
#include "../Things/Structures/Structures.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <sstream>
#include <vector>

// Disable some warnings that can be safely ignored.
#pragma warning(disable: 4244) // possible loss of data (floats to int and vice versa)


using namespace NAS2D;


const std::string MAP_TERRAIN_EXTENSION = "_a.png";
const std::string MAP_DISPLAY_EXTENSION = "_b.png";

extern Point<int> MOUSE_COORDS;


Rectangle<int> RESOURCE_PANEL_PIN{0, 1, 8, 19};
Rectangle<int> POPULATION_PANEL_PIN{675, 1, 8, 19};

std::string CURRENT_LEVEL_STRING;

std::map <int, std::string> LEVEL_STRING_TABLE = 
{
	{ constants::DEPTH_SURFACE, constants::LEVEL_SURFACE },
	{ constants::DEPTH_UNDERGROUND_1, constants::LEVEL_UG1 },
	{ constants::DEPTH_UNDERGROUND_2, constants::LEVEL_UG2 },
	{ constants::DEPTH_UNDERGROUND_3, constants::LEVEL_UG3 },
	{ constants::DEPTH_UNDERGROUND_4, constants::LEVEL_UG4 }
};


const Font* MAIN_FONT = nullptr;


MapViewState::MapViewState(MainReportsUiState& mainReportsState, const std::string& savegame) :
	mMainReportsState(mainReportsState),
	mLoadingExisting(true),
	mExistingToLoad(savegame)
{
	ccLocation() = CcNotPlaced;
	Utility<EventHandler>::get().windowResized().connect(this, &MapViewState::onWindowResized);
}


MapViewState::MapViewState(MainReportsUiState& mainReportsState, const Planet::Attributes& planetAttributes) :
	mMainReportsState(mainReportsState),
	mTileMap(new TileMap(planetAttributes.mapImagePath, planetAttributes.tilesetPath, planetAttributes.maxDepth, planetAttributes.maxMines, planetAttributes.hostility)),
	mPlanetAttributes(planetAttributes),
	mMapDisplay{std::make_unique<Image>(planetAttributes.mapImagePath + MAP_DISPLAY_EXTENSION)},
	mHeightMap{std::make_unique<Image>(planetAttributes.mapImagePath + MAP_TERRAIN_EXTENSION)}
{
	ccLocation() = CcNotPlaced;
	Utility<EventHandler>::get().windowResized().connect(this, &MapViewState::onWindowResized);
}


MapViewState::~MapViewState()
{
	delete mPathSolver;

	scrubRobotList();
	delete mTileMap;

	Utility<Renderer>::get().setCursor(PointerType::POINTER_NORMAL);

	EventHandler& e = Utility<EventHandler>::get();
	e.activate().disconnect(this, &MapViewState::onActivate);
	e.keyDown().disconnect(this, &MapViewState::onKeyDown);
	e.mouseButtonDown().disconnect(this, &MapViewState::onMouseDown);
	e.mouseButtonUp().disconnect(this, &MapViewState::onMouseUp);
	e.mouseDoubleClick().disconnect(this, &MapViewState::onMouseDoubleClick);
	e.mouseMotion().disconnect(this, &MapViewState::onMouseMove);
	e.mouseWheel().disconnect(this, &MapViewState::onMouseWheel);
	e.windowResized().disconnect(this, &MapViewState::onWindowResized);

	e.textInputMode(false);
}


void MapViewState::setPopulationLevel(PopulationLevel popLevel)
{
	mLandersColonist = static_cast<int>(popLevel);
	mLandersCargo = 2; ///\todo This should be set based on difficulty level.
}


/**
 * Initialize values, the UI and set up event handling.
 */
void MapViewState::initialize()
{
	// UI
	initUi();
	auto& renderer = Utility<Renderer>::get();

	renderer.setCursor(PointerType::POINTER_NORMAL);

	setupUiPositions(renderer.size());

	CURRENT_LEVEL_STRING = constants::LEVEL_SURFACE;

	mPopulationPool.population(&mPopulation);

	if (mLoadingExisting) 
	{ 
		load(mExistingToLoad); 
	}
	else 
	{ 
		// StructureCatalogue is initialized in load routine if saved game present to load existing structures
		StructureCatalogue::init(mPlanetAttributes.meanSolarDistance); 
	}

	//Utility<Mixer>::get().fadeInMusic(mBgMusic);
	Utility<Renderer>::get().fadeIn(constants::FADE_SPEED);

	EventHandler& e = Utility<EventHandler>::get();

	e.activate().connect(this, &MapViewState::onActivate);
	e.keyDown().connect(this, &MapViewState::onKeyDown);
	e.mouseButtonDown().connect(this, &MapViewState::onMouseDown);
	e.mouseButtonUp().connect(this, &MapViewState::onMouseUp);
	e.mouseDoubleClick().connect(this, &MapViewState::onMouseDoubleClick);
	e.mouseMotion().connect(this, &MapViewState::onMouseMove);
	e.mouseWheel().connect(this, &MapViewState::onMouseWheel);

	e.textInputMode(true);

	MAIN_FONT = &fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);

	delete mPathSolver;
	mPathSolver = new micropather::MicroPather(mTileMap);
}


void MapViewState::_activate()
{
	unhideUi();
}


void MapViewState::_deactivate()
{
	mGameOverDialog.enabled(false);
	mGameOptionsDialog.enabled(false);

	hideUi();
}


void MapViewState::focusOnStructure(Structure* s)
{
	if (!s) { return; }
	mTileMap->centerMapOnTile(&Utility<StructureManager>::get().tileFromStructure(s));
}


/**
 * Updates the entire state of the game.
 */
State* MapViewState::update()
{
	auto& renderer = Utility<Renderer>::get();
	const auto renderArea = NAS2D::Rectangle<int>::Create({0, 0}, renderer.size());

	// Game's over, don't bother drawing anything else
	if (mGameOverDialog.visible())
	{
		renderer.drawBoxFilled(renderArea, NAS2D::Color::Black);
		mGameOverDialog.update();

		return this;
	}

	renderer.drawImageStretched(mBackground, renderArea);

	// explicit current level
	const Font* font = &fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM);
	const auto currentLevelPosition = mMiniMapBoundingBox.crossXPoint() - font->size(CURRENT_LEVEL_STRING) - NAS2D::Vector{0, 12};
	renderer.drawText(*font, CURRENT_LEVEL_STRING, currentLevelPosition, NAS2D::Color::White);

	if (!modalUiElementDisplayed())
	{
		mTileMap->injectMouse(MOUSE_COORDS);
	}

	mTileMap->draw();

	// FIXME: Ugly / hacky
	if (modalUiElementDisplayed())
	{
		renderer.drawBoxFilled(renderArea, NAS2D::Color{0, 0, 0, 165});
	}

	drawUI();

	return this;
}


/**
 * Get the total amount of storage given a structure class and capacity of each
 * structure.
 */
int MapViewState::totalStorage(Structure::StructureClass structureClass, int capacity)
{
	int storageCapacity = 0;

	// Command Center has a limited amount of storage for when colonists first land.
	if (ccLocation() != CcNotPlaced)
	{
		storageCapacity += constants::BASE_STORAGE_CAPACITY;
	}

	const auto& structures = Utility<StructureManager>::get().structureList(structureClass);
	for (auto structure : structures)
	{
		if (structure->operational() || structure->isIdle())
		{
			storageCapacity += capacity;
		}
	}

	return storageCapacity;
}


int MapViewState::refinedResourcesInStorage()
{
	int total = 0;
	for (size_t i = 0; i < mPlayerResources.resources.size(); ++i)
	{
		total += mPlayerResources.resources[i];
	}

	auto commandCenterResources = mTileMap->getTile(ccLocation()).structure()->storage();
	for (size_t i = 0; i < commandCenterResources.resources.size(); ++i)
	{
		total += commandCenterResources.resources[i];
	}

	return total;
}


void MapViewState::updatePlayerResources()
{
	StructureList storage = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Storage);
	storage.insert(storage.begin(), mTileMap->getTile(ccLocation()).structure());

	mPlayerResources.clear();

	for (auto structure : storage)
	{
		mPlayerResources += structure->storage();
	}
}


/**
 * Window activation handler.
 */
void MapViewState::onActivate(bool /*newActiveValue*/)
{
	mLeftButtonDown = false;
}


/**
 *
 */
void MapViewState::onWindowResized(int w, int h)
{
	setupUiPositions({w, h});
	mTileMap->initMapDrawParams({w, h});
}


/**
 * Key down event handler.
 */
void MapViewState::onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool /*repeat*/)
{
	if (!active()) { return; }

	// FIXME: Ugly / hacky
	if (modalUiElementDisplayed())
	{
		return;
	}

	if (key == EventHandler::KeyCode::KEY_F1)
	{
		mReportsUiCallback();
		return;
	}

	bool viewUpdated = false; // don't like flaggy code like this
	Point<int> pt = mTileMap->mapViewLocation();

	switch(key)
	{
		case EventHandler::KeyCode::KEY_w:
		case EventHandler::KeyCode::KEY_UP:
			viewUpdated = true;
			pt += DirectionNorth;
			break;

		case EventHandler::KeyCode::KEY_s:
		case EventHandler::KeyCode::KEY_DOWN:
			viewUpdated = true;
			pt += DirectionSouth;
			break;

		case EventHandler::KeyCode::KEY_a:
		case EventHandler::KeyCode::KEY_LEFT:
			viewUpdated = true;
			pt += DirectionWest;
			break;

		case EventHandler::KeyCode::KEY_d:
		case EventHandler::KeyCode::KEY_RIGHT:
			viewUpdated = true;
			pt += DirectionEast;
			break;

		case EventHandler::KeyCode::KEY_0:
			viewUpdated = true;
			changeViewDepth(0);
			break;

		case EventHandler::KeyCode::KEY_1:
			viewUpdated = true;
			changeViewDepth(1);
			break;

		case EventHandler::KeyCode::KEY_2:
			viewUpdated = true;
			changeViewDepth(2);
			break;

		case EventHandler::KeyCode::KEY_3:
			viewUpdated = true;
			changeViewDepth(3);
			break;

		case EventHandler::KeyCode::KEY_4:
			viewUpdated = true;
			changeViewDepth(4);
			break;

		case EventHandler::KeyCode::KEY_PAGEUP:
			viewUpdated = true;
			changeViewDepth(mTileMap->currentDepth() - 1);
			break;

		case EventHandler::KeyCode::KEY_PAGEDOWN:
			viewUpdated = true;
			changeViewDepth(mTileMap->currentDepth() + 1);
			break;


		case EventHandler::KeyCode::KEY_HOME:
			viewUpdated = true;
			changeViewDepth(0);
			break;

		case EventHandler::KeyCode::KEY_END:
			viewUpdated = true;
			changeViewDepth(mTileMap->maxDepth());
			break;

		case EventHandler::KeyCode::KEY_F10:
			if (Utility<EventHandler>::get().control(mod) && Utility<EventHandler>::get().shift(mod))
			{
				StorableResources resourcesToAdd{ 1000, 1000, 1000, 1000 };
				addRefinedResources(resourcesToAdd);
				updatePlayerResources();
				updateStructuresAvailability();
			}
			break;

		case EventHandler::KeyCode::KEY_F2:
			mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
			mFileIoDialog.setMode(FileIo::FileOperation::FILE_SAVE);
			mFileIoDialog.show();
			break;

		case EventHandler::KeyCode::KEY_F3:
			mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
			mFileIoDialog.setMode(FileIo::FileOperation::FILE_LOAD);
			mFileIoDialog.show();
			break;

		case EventHandler::KeyCode::KEY_ESCAPE:
			clearMode();
			resetUi();
			break;

		case EventHandler::KeyCode::KEY_ENTER:
			if (mBtnTurns.enabled()) { nextTurn(); }
			break;

		default:
			break;
	}

	if (viewUpdated)
	{
		mTileMap->mapViewLocation(pt);
	}
}


/**
 * Mouse Down event handler.
 */
void MapViewState::onMouseDown(EventHandler::MouseButton button, int /*x*/, int /*y*/)
{
	if (!active()) { return; }

	if (modalUiElementDisplayed()) { return; }

	if (mWindowStack.pointInWindow(MOUSE_COORDS))
	{
		mWindowStack.updateStack(MOUSE_COORDS);
		return;
	}

	if (button == EventHandler::MouseButton::BUTTON_RIGHT)
	{
		if (mInsertMode != InsertMode::None)
		{
			resetUi();
			return;
		}

		if (!mTileMap->tileHighlightVisible()) { return; }
		if (!mTileMap->isValidPosition(mTileMap->tileMouseHover())) { return; }

		auto& tile = mTileMap->getTile(mTileMap->tileMouseHover());
		if (tile.empty() && mTileMap->boundingBox().contains(MOUSE_COORDS))
		{
			clearSelections();
			mTileInspector.tile(&tile);
			mTileInspector.show();
			mWindowStack.bringToFront(&mTileInspector);
		}
		else if (tile.thingIsStructure())
		{
			Structure* structure = tile.structure();

			if (structure->isFactory() && (structure->operational() || structure->isIdle()))
			{
				mFactoryProduction.factory(static_cast<Factory*>(structure));
				mFactoryProduction.show();
				mWindowStack.bringToFront(&mFactoryProduction);
			}
			else if (structure->isWarehouse() && (structure->operational() || structure->isIdle()))
			{
				mWarehouseInspector.warehouse(static_cast<Warehouse*>(structure));
				mWarehouseInspector.show();
				mWindowStack.bringToFront(&mWarehouseInspector);
			}
			else if (structure->isMineFacility() && (structure->operational() || structure->isIdle()))
			{
				mMineOperationsWindow.mineFacility(static_cast<MineFacility*>(structure));
				mMineOperationsWindow.show();
				mWindowStack.bringToFront(&mMineOperationsWindow);
			}
			else
			{
				mStructureInspector.structure(structure);
				mStructureInspector.show();
				mWindowStack.bringToFront(&mStructureInspector);
			}
		}
	}

	if (button == EventHandler::MouseButton::BUTTON_LEFT)
	{
		mLeftButtonDown = true;

		Point<int> pt = mTileMap->mapViewLocation();

		if (mMenuIconRect.contains(MOUSE_COORDS))
		{
			mGameOptionsDialog.show();
			resetUi();
			return;
		}

		if (RESOURCE_PANEL_PIN.contains(MOUSE_COORDS)) { mPinResourcePanel = !mPinResourcePanel; }
		if (POPULATION_PANEL_PIN.contains(MOUSE_COORDS)) { mPinPopulationPanel = !mPinPopulationPanel; }

		if (mMoveNorthIconRect.contains(MOUSE_COORDS))
		{
			mTileMap->mapViewLocation(pt + DirectionNorth);
		}
		else if (mMoveSouthIconRect.contains(MOUSE_COORDS))
		{
			mTileMap->mapViewLocation(pt + DirectionSouth);
		}
		else if (mMoveEastIconRect.contains(MOUSE_COORDS))
		{
			mTileMap->mapViewLocation(pt + DirectionEast);
		}
		else if (mMoveWestIconRect.contains(MOUSE_COORDS))
		{
			mTileMap->mapViewLocation(pt + DirectionWest);
		}
		else if (mMoveUpIconRect.contains(MOUSE_COORDS))
		{
			changeViewDepth(mTileMap->currentDepth() - 1);
		}
		else if (mMoveDownIconRect.contains(MOUSE_COORDS))
		{
			changeViewDepth(mTileMap->currentDepth()+1);
		}

		// MiniMap Check
		if (mMiniMapBoundingBox.contains(MOUSE_COORDS) && !mWindowStack.pointInWindow(MOUSE_COORDS))
		{
			setMinimapView();
		}
		// Click was within the bounds of the TileMap.
		else if (mTileMap->boundingBox().contains(MOUSE_COORDS))
		{
			EventHandler& e = Utility<EventHandler>::get();
			if (mInsertMode == InsertMode::Structure)
			{
				placeStructure();
			}
			else if (mInsertMode == InsertMode::Robot)
			{
				placeRobot();
			}
			else if ( (mInsertMode == InsertMode::Tube) && e.query_shift())
			{
				placeTubeStart();
			}
			else if (mInsertMode == InsertMode::Tube)
			{
				placeTubes();
			}
		}
	}
}


void MapViewState::onMouseDoubleClick(EventHandler::MouseButton button, int /*x*/, int /*y*/)
{
	if (!active()) { return; }

	if (button == EventHandler::MouseButton::BUTTON_LEFT)
	{
		if (mWindowStack.pointInWindow(MOUSE_COORDS)) { return; }
		if (!mTileMap->tileHighlightVisible()) { return; }
		if (!mTileMap->isValidPosition(mTileMap->tileMouseHover())) { return; }

		auto& tile = mTileMap->getTile(mTileMap->tileMouseHover());
		if (tile.thingIsStructure())
		{
			Structure* structure = tile.structure();

			if (structure->isFactory()) { mMainReportsState.selectFactoryPanel(structure); }
			else if (structure->isWarehouse()) { mMainReportsState.selectWarehousePanel(structure); }
			else if (structure->isMineFacility() || structure->structureClass() == Structure::StructureClass::Smelter) { mMainReportsState.selectMinePanel(structure); }
			else { return; } // avoids showing the full-screen UI on unhandled structures.

			mReportsUiCallback();
		}
	}
}


/**
* Mouse Up event handler.
*/
void MapViewState::onMouseUp(EventHandler::MouseButton button, int /*x*/, int /*y*/)
{
	if (button == EventHandler::MouseButton::BUTTON_LEFT)
	{
		mLeftButtonDown = false;
		EventHandler& e = Utility<EventHandler>::get();
		if ((mInsertMode == InsertMode::Tube) && e.query_shift())
		{
			placeTubeEnd();
		}
	}
}


/**
* Mouse motion event handler.
*/
void MapViewState::onMouseMove(int /*x*/, int /*y*/, int /*rX*/, int /*rY*/)
{
	if (!active()) { return; }


	if (mLeftButtonDown)
	{
		if (mMiniMapBoundingBox.contains(MOUSE_COORDS))
		{
			setMinimapView();
		}
	}

	mTileMapMouseHover = mTileMap->tileMouseHover();
}


/**
 * Mouse wheel event handler.
 */
void MapViewState::onMouseWheel(int /*x*/, int y)
{
	if (mInsertMode != InsertMode::Tube) { return; }

	y > 0 ? mConnections.decrementSelection() : mConnections.incrementSelection();
}


/**
 * Changes the current view depth.
 */
void MapViewState::changeViewDepth(int depth)
{
	mTileMap->currentDepth(depth);

	if (mInsertMode != InsertMode::Robot) { clearMode(); }
	populateStructureMenu();
	updateCurrentLevelString(mTileMap->currentDepth());
}


/**
 * 
 */
void MapViewState::setMinimapView()
{
	const auto viewSizeInTiles = NAS2D::Vector{mTileMap->edgeLength(), mTileMap->edgeLength()};
	const auto position = NAS2D::Point{0, 0} + (MOUSE_COORDS - mMiniMapBoundingBox.startPoint()) - viewSizeInTiles / 2;

	mTileMap->mapViewLocation(position);
}


/**
 * Clears the build mode.
 */
void MapViewState::clearMode()
{
	mInsertMode = InsertMode::None;
	Utility<Renderer>::get().setCursor(PointerType::POINTER_NORMAL);

	mCurrentStructure = StructureID::SID_NONE;
	mCurrentRobot = RobotType::ROBOT_NONE;

	clearSelections();
}


/**
 * 
 */
void MapViewState::insertTube(ConnectorDir dir, int depth, Tile* tile)
{

	if (dir == ConnectorDir::CONNECTOR_VERTICAL)
	{
		throw std::runtime_error("MapViewState::insertTube() called with invalid ConnectorDir paramter.");
	}

	Utility<StructureManager>::get().addStructure(new Tube(dir, depth != 0), tile);
}


/**
 * 
 */
void MapViewState::placeTubes()
{
	Tile* tile = mTileMap->getVisibleTile(mTileMapMouseHover, mTileMap->currentDepth());
	if (!tile) { return; }

	// Check the basics.
	if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated()) { return; }

	/** \fixme	This is a kludge that only works because all of the tube structures are listed alphabetically.
	 *			Should instead take advantage of the updated meta data in the IconGridItem.
	 */
	auto cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);

	if (validTubeConnection(mTileMap, mTileMapMouseHover, cd))
	{
		insertTube(cd, mTileMap->currentDepth(), &mTileMap->getTile(mTileMapMouseHover));

		// FIXME: Naive approach -- will be slow with larger colonies.
		Utility<StructureManager>::get().disconnectAll();
		checkConnectedness();
	}
	else
	{
		doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_TUBE_INVALID_LOCATION);
	}
}

/**
 *
 */
void MapViewState::placeTubeStart()
{
	mPlacingTube = false;

	Tile* tile = mTileMap->getVisibleTile(mTileMapMouseHover, mTileMap->currentDepth());
	if (!tile) { return; }

	// Check the basics.
	if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated()) { return; }

	/** \fixme	This is a kludge that only works because all of the tube structures are listed alphabetically.
	 *			Should instead take advantage of the updated meta data in the IconGridItem.
	 */
	ConnectorDir cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);

	if (!validTubeConnection(mTileMap, mTileMapMouseHover, cd))
	{
		doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_TUBE_INVALID_LOCATION);
		return;
	}
	mTubeStart = tile->position();
	mPlacingTube = true;
}

/**
 *
 */
void MapViewState::placeTubeEnd()
{
	if (!mPlacingTube) return;
	mPlacingTube = false;
	Tile* tile = mTileMap->getVisibleTile(mTileMapMouseHover, mTileMap->currentDepth());
	if (!tile) { return; }

	/** \fixme	This is a kludge that only works because all of the tube structures are listed alphabetically.
	 *			Should instead take advantage of the updated meta data in the IconGridItem.
	 */
	ConnectorDir cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);

	const auto startEndDirection = tile->position() - mTubeStart;
	NAS2D::Vector<int> tubeEndOffset;

	switch (cd)
	{
	case ConnectorDir::CONNECTOR_INTERSECTION:
		// Determine direction of largest change, and snap to that axis
		if (abs(startEndDirection.x) >= abs(startEndDirection.y)){
			tubeEndOffset = {startEndDirection.x, 0};
		}else{
			tubeEndOffset = {0, startEndDirection.y};
		}
		break;
	case ConnectorDir::CONNECTOR_RIGHT:
		tubeEndOffset = {startEndDirection.x, 0};
		break;
	case ConnectorDir::CONNECTOR_LEFT:
		tubeEndOffset = {0, startEndDirection.y};
		break;
	default:
		return;
	}
	// Tube is axis aligned, so either x or y is 0
	const int tubeLength = abs(tubeEndOffset.x + tubeEndOffset.y);
	const auto tubeDirection = tubeEndOffset / tubeLength;
	const auto tubeEnd = mTubeStart + tubeEndOffset;

	auto position = mTubeStart;
	bool endReach = false;

	do {
		tile = mTileMap->getVisibleTile(mTubeStart, mTileMap->currentDepth());
		if (!tile) {
			endReach = true;
		}else if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated()){
			endReach = true;
		}else if (!validTubeConnection(mTileMap, position, cd)){
			endReach = true;
		}else{
			insertTube(cd, mTileMap->currentDepth(), &mTileMap->getTile(position));

			// FIXME: Naive approach -- will be slow with larger colonies.
			Utility<StructureManager>::get().disconnectAll();
			checkConnectedness();
		}

		if (position == tubeEnd) endReach = true;
		position += tubeDirection;
	} while (!endReach);
}

/**
 * 
 */
void MapViewState::placeRobot()
{
	Tile* tile = mTileMap->getVisibleTile();
	if (!tile) { return; }
	if (!mRobotPool.robotCtrlAvailable()) { return; }
	
	if (!inCommRange(tile->position()))
	{
		doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_OUT_OF_COMM_RANGE);
		return;
	}

	if(mCurrentRobot == RobotType::ROBOT_DOZER)
	{
		Robot* robot = mRobotPool.getDozer();

		if (!tile->excavated() || (tile->thing() && !tile->thingIsStructure()))
		{
			return;
		}
		else if (tile->index() == TerrainType::Dozed && !tile->thingIsStructure())
		{
			doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_TILE_BULLDOZED);
			return;
		}
		else if (tile->mine())
		{
			if (tile->mine()->depth() != mTileMap->maxDepth() || !tile->mine()->exhausted())
			{
				doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_MINE_NOT_EXHAUSTED);
				return;
			}

			mMineOperationsWindow.hide();
			mTileMap->removeMineLocation(mTileMap->tileMouseHover());
			tile->pushMine(nullptr);
			for (int i = 0; i <= mTileMap->maxDepth(); ++i)
			{
				auto& mineShaftTile = mTileMap->getTile(mTileMap->tileMouseHover(), i);
				Utility<StructureManager>::get().removeStructure(mineShaftTile.structure());
			}
		}
		else if (tile->thingIsStructure())
		{
			if (mStructureInspector.structure() == tile->structure()) { mStructureInspector.hide(); }
			
			Structure* structure = tile->structure();

			if (structure->isMineFacility()) { return; }
			if (structure->structureClass() == Structure::StructureClass::Command)
			{
				doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_CANNOT_BULLDOZE_CC);
				return;
			}

			if (structure->structureClass() == Structure::StructureClass::Lander && structure->age() == 0)
			{
				doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_CANNOT_BULLDOZE_LANDING_SITE);
				return;
			}

			if (structure->isRobotCommand())
			{
				deleteRobotsInRCC(robot, static_cast<RobotCommand*>(structure), mRobotPool, mRobotList, tile);
			}

			if (structure->isFactory() && static_cast<Factory*>(structure) == mFactoryProduction.factory())
			{
				mFactoryProduction.hide();
			}

			if (structure->isWarehouse())
			{
				if (simulateMoveProducts(static_cast<Warehouse*>(structure))) { moveProducts(static_cast<Warehouse*>(structure)); }
				else { return; }
			}

			/**
			 * \fixme	Since the StructureTranslator class will be deprecated in the future, there needs to be a better
			 * 			way to determine this. I may go back to defining recycling values in the individual structures
			 * 			themselves but I'm still not sure I love that idea. Will have to think about that one a bit.
			 */
			mPlayerResources = mPlayerResources + StructureCatalogue::recyclingValue(structure->structureId());
			updateStructuresAvailability();

			tile->connected(false);
			Utility<StructureManager>::get().removeStructure(structure);
			tile->deleteThing();
			Utility<StructureManager>::get().disconnectAll();
			static_cast<Robodozer*>(robot)->tileIndex(static_cast<std::size_t>(TerrainType::Dozed));
			checkConnectedness();
		}

		int taskTime = tile->index() == TerrainType::Dozed ? 1 : static_cast<int>(tile->index());
		robot->startTask(taskTime);
		mRobotPool.insertRobotIntoTable(mRobotList, robot, tile);
		static_cast<Robodozer*>(robot)->tileIndex(static_cast<std::size_t>(tile->index()));
		tile->index(TerrainType::Dozed);

		if(!mRobotPool.robotAvailable(RobotType::ROBOT_DOZER))
		{
			mRobots.removeItem(constants::ROBODOZER);
			clearMode();
		}
	}
	else if(mCurrentRobot == RobotType::ROBOT_DIGGER)
	{
		// Keep digger within a safe margin of the map boundaries.
		if (!NAS2D::Rectangle<int>::Create({4, 4}, NAS2D::Point{-4, -4} + mTileMap->size()).contains(mTileMapMouseHover))
		{
			doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_DIGGER_EDGE_BUFFER);
			return;
		}

		if (!tile->excavated()) { return; }

		// Check for obstructions underneath the the digger location.
		if (tile->depth() != mTileMap->maxDepth() && !mTileMap->getTile(tile->position(), tile->depth() + 1).empty())
		{
			doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_DIGGER_BLOCKED_BELOW);
			return;
		}

		if (tile->hasMine())
		{
			if (!doYesNoMessage(constants::ALERT_DIGGER_MINE_TITLE, constants::ALERT_DIGGER_MINE)) { return; }

			const auto position = tile->position();
			std::cout << "Digger destroyed a Mine at (" << position.x << ", " << position.y << ")." << std::endl;
			mTileMap->removeMineLocation(position);
		}

		// Die if tile is occupied or not excavated.
		if (!tile->empty())
		{
			if (tile->depth() > constants::DEPTH_SURFACE)
			{
				if (tile->thingIsStructure() && tile->structure()->connectorDirection() != ConnectorDir::CONNECTOR_VERTICAL) //air shaft
				{
					doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_STRUCTURE_IN_WAY);
					return;
				}
				else if (tile->thingIsStructure() && tile->structure()->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL && tile->depth() == mTileMap->maxDepth())
				{
					doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_MAX_DIG_DEPTH);
					return;
				}
			}
			else
			{
				doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_STRUCTURE_IN_WAY);
				return;
			}
		}

		if (!tile->thing() && mTileMap->currentDepth() > 0) { mDiggerDirection.cardinalOnlyEnabled(); }
		else { mDiggerDirection.downOnlyEnabled(); }

		mDiggerDirection.setParameters(tile);

		// NOTE:	Unlike the Dozer and Miner, Diggers aren't removed here but instead
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

			// Popup to the right of the mouse
			auto position = MOUSE_COORDS + NAS2D::Vector{20, -32};
			// Check if popup position is off the right edge of the display area
			if (position.x + mDiggerDirection.size().x > Utility<Renderer>::get().size().x)
			{
				// Popup to the left of the mouse
				position = MOUSE_COORDS + NAS2D::Vector{-20 - mDiggerDirection.size().x, -32};
			}
			mDiggerDirection.position(position);
		}
	}
	else if(mCurrentRobot == RobotType::ROBOT_MINER)
	{
		if (tile->thing()) { doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_MINER_TILE_OBSTRUCTED); return; }
		if (mTileMap->currentDepth() != constants::DEPTH_SURFACE) { doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_MINER_SURFACE_ONLY); return; }
		if (!tile->mine()) { doAlertMessage(constants::ALERT_INVALID_ROBOT_PLACEMENT, constants::ALERT_MINER_NOT_ON_MINE); return; }

		Robot* robot = mRobotPool.getMiner();
		robot->startTask(constants::MINER_TASK_TIME);
		mRobotPool.insertRobotIntoTable(mRobotList, robot, tile);
		tile->index(TerrainType::Dozed);

		if (!mRobotPool.robotAvailable(RobotType::ROBOT_MINER))
		{
			mRobots.removeItem(constants::ROBOMINER);
			clearMode();
		}
	}
}


/**
 * Checks the robot selection interface and if the robot is not available in it, adds
 * it back in.
 */
void MapViewState::checkRobotSelectionInterface(const std::string& rType, int sheetIndex, RobotType _rid)
{
	if (!mRobots.itemExists(rType))
	{
		mRobots.addItemSorted(rType, sheetIndex, _rid);
	}
}


/**
 * Places a structure into the map.
 */
void MapViewState::placeStructure()
{
	if (mCurrentStructure == StructureID::SID_NONE) { throw std::runtime_error("MapViewState::placeStructure() called but mCurrentStructure == STRUCTURE_NONE"); }

	Tile* tile = mTileMap->getVisibleTile();
	if (!tile) { return; }

	if (!structureIsLander(mCurrentStructure) && !selfSustained(mCurrentStructure) &&
		(tile->distanceTo(&mTileMap->getTile(ccLocation(), 0)) > constants::ROBOT_COM_RANGE))
	{
		doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_STRUCTURE_OUT_OF_RANGE);
		return;
	}

	if (tile->mine())
	{
		doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_STRUCTURE_MINE_IN_WAY);
		return;
	}
	
	if (tile->thing())
	{
		if (tile->thingIsStructure())
		{
			doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_STRUCTURE_TILE_OBSTRUCTED);
		}
		else
		{
			doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_STRUCTURE_TILE_THING);
		}
		return;
	}

	if((!tile->bulldozed() && !structureIsLander(mCurrentStructure)))
	{
		doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_STRUCTURE_TERRAIN);
		return;
	}

	if (!tile->excavated())
	{
		doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_STRUCTURE_EXCAVATED);
		return;
	}

	// Seed lander is a special case and only one can ever be placed by the player ever.
	if(mCurrentStructure == StructureID::SID_SEED_LANDER)
	{
		insertSeedLander(mTileMapMouseHover);
	}
	else if (mCurrentStructure == StructureID::SID_COLONIST_LANDER)
	{
		if (!validLanderSite(*tile)) { return; }

		ColonistLander* s = new ColonistLander(tile);
		s->deployCallback().connect(this, &MapViewState::deployColonistLander);
		Utility<StructureManager>::get().addStructure(s, tile);

		--mLandersColonist;
		if (mLandersColonist == 0)
		{
			clearMode();
			resetUi();
			populateStructureMenu();
		}
	}
	else if (mCurrentStructure == StructureID::SID_CARGO_LANDER)
	{
		if (!validLanderSite(*tile)) { return; }

		CargoLander* _lander = new CargoLander(tile);
		_lander->deployCallback().connect(this, &MapViewState::deployCargoLander);
		Utility<StructureManager>::get().addStructure(_lander, tile);

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
		if (!validStructurePlacement(mTileMap, mTileMapMouseHover) && !selfSustained(mCurrentStructure))
		{
			doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, constants::ALERT_STRUCTURE_NO_TUBE);
			return;
		}

		// Check build cost
		if (!StructureCatalogue::canBuild(mPlayerResources, mCurrentStructure))
		{
			resourceShortageMessage(mPlayerResources, mCurrentStructure);
			return;
		}

		Structure* structure = StructureCatalogue::get(mCurrentStructure);
		if (!structure) { throw std::runtime_error("MapViewState::placeStructure(): NULL Structure returned from StructureCatalog."); }

		Utility<StructureManager>::get().addStructure(structure, tile);

		// FIXME: Ugly
		if (structure->isFactory())
		{
			static_cast<Factory*>(structure)->productionComplete().connect(this, &MapViewState::factoryProductionComplete);
			static_cast<Factory*>(structure)->resourcePool(&mPlayerResources);
		}

		mPlayerResources = mPlayerResources - StructureCatalogue::costToBuild(mCurrentStructure);
		updateStructuresAvailability();
	}
}


/**
 * Checks that the clicked tile is a suitable spot for the SEED Lander and
 * then inserts it into the the TileMap.
 */
void MapViewState::insertSeedLander(NAS2D::Point<int> point)
{
	// Has to be built away from the edges of the map
	if (NAS2D::Rectangle<int>::Create({4, 4}, NAS2D::Point{-4, -4} + mTileMap->size()).contains(point))
	{
		// check for obstructions
		if (!landingSiteSuitable(mTileMap, point))
		{
			return;
		}

		SeedLander* s = new SeedLander(point);
		s->deployCallback().connect(this, &MapViewState::deploySeedLander);
		Utility<StructureManager>::get().addStructure(s, &mTileMap->getTile(point)); // Can only ever be placed on depth level 0

		clearMode();
		resetUi();

		mStructures.dropAllItems();
		mBtnTurns.enabled(true);
	}
	else
	{
		doAlertMessage(constants::ALERT_LANDER_LOCATION, constants::ALERT_SEED_EDGE_BUFFER);
	}
}


/**
 * Updates all robots.
 */
void MapViewState::updateRobots()
{
	auto robot_it = mRobotList.begin();
	while(robot_it != mRobotList.end())
	{
		auto robot = robot_it->first;
		auto tile = robot_it->second;

		robot->update();

		if (robot->dead())
		{
			std::cout << "dead robot" << std::endl;

			// \fixme	This is an awful way of doing this.
			if (robot->name() != constants::ROBOMINER)
			{
				const auto position = tile->position();
				const auto robotLocationText = std::to_string(position.x) + ", " + std::to_string(position.y);
				const auto text = "Your " + robot->name() + " at location " + robotLocationText + " has broken down. It will not be able to complete its task and will be removed from your inventory.";
				doAlertMessage("Robot Breakdown", text);
				Robodozer* _d = dynamic_cast<Robodozer*>(robot);
				if (_d) { tile->index(static_cast<TerrainType>(_d->tileIndex())); }
			}

			if (tile->thing() == robot)
			{
				tile->removeThing();
			}

			/// \fixme	Brute force.
			for (auto rcc : Utility<StructureManager>::get().structureList(Structure::StructureClass::RobotCommand))
			{
				static_cast<RobotCommand*>(rcc)->removeRobot(robot);
			}

			mRobotPool.erase(robot);
			delete robot;
			robot_it = mRobotList.erase(robot_it);
		}
		else if(robot->idle())
		{
			if (tile->thing() == robot)
			{
				tile->removeThing();
			}

			robot_it = mRobotList.erase(robot_it);
		}
		else
		{
			++robot_it;
		}
	}

	updateRobotControl(mRobotPool);
}


/**
 * Checks and sets the current structure mode.
 */
void MapViewState::setStructureID(StructureID type, InsertMode mode)
{

	if (type == StructureID::SID_NONE)
	{
		clearMode();
		return;
	}

	mCurrentStructure = type;

	mInsertMode = mode;
	Utility<Renderer>::get().setCursor(PointerType::POINTER_PLACE_TILE);

}


/**
 * Checks the connectedness of all tiles surrounding
 * the Command Center.
 */
void MapViewState::checkConnectedness()
{
	if (ccLocation() == CcNotPlaced)
	{
		return;
	}

	// Assumes that the 'thing' at mCCLocation is in fact a Structure.
	auto& tile = mTileMap->getTile(ccLocation(), 0);
	Structure *cc = tile.structure();

	if (!cc)
	{
		throw std::runtime_error("CC coordinates do not actually point to a Command Center.");
	}

	if (cc->state() == StructureState::UnderConstruction)
	{
		return;
	}

	tile.connected(true);

	// Start graph walking at the CC location.
	GraphWalker graphWalker(ccLocation(), 0, mTileMap);
}


/**
 * Removes deployed robots from the TileMap to
 * prevent dangling pointers. Yay for raw memory!
 */
void MapViewState::scrubRobotList()
{
	for (auto it : mRobotList)
	{
		it.second->removeThing();
	}
}


/**
 * Update the value of the current level string
 */
void MapViewState::updateCurrentLevelString(int currentDepth)
{
	CURRENT_LEVEL_STRING = LEVEL_STRING_TABLE[currentDepth];
}
