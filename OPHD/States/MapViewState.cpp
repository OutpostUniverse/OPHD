#include "MapViewState.h"

#include "MainMenuState.h"
#include "MainReportsUiState.h"
#include "Route.h"

#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include "../DirectionOffset.h"
#include "../Cache.h"
#include "../GraphWalker.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"

#include "../Map/Tile.h"
#include "../Map/TileMap.h"

#include "../Things/Robots/Robots.h"
#include "../Things/Structures/Structures.h"

#include "../UI/MessageBox.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <sstream>
#include <vector>

// Disable some warnings that can be safely ignored.
#pragma warning(disable : 4244) // possible loss of data (floats to int and vice versa)


extern NAS2D::Point<int> MOUSE_COORDS;


NAS2D::Rectangle<int> RESOURCE_PANEL_PIN{0, 1, 8, 19};
NAS2D::Rectangle<int> POPULATION_PANEL_PIN{675, 1, 8, 19};

const NAS2D::Font* MAIN_FONT = nullptr;


/** \fixme Find a sane place for these */
struct RobotMeta
{
	std::string name;
	const int sheetIndex;
};

const std::map<Robot::Type, RobotMeta> RobotMetaTable
{
	{Robot::Type::Digger, RobotMeta{constants::Robodigger, constants::RobodiggerSheetId}},
	{Robot::Type::Dozer, RobotMeta{constants::Robodozer, constants::RobodozerSheetId}},
	{Robot::Type::Miner, RobotMeta{constants::Robominer, constants::RobominerSheetId}}
};


static NAS2D::Rectangle<int> buildAreaRectFromTile(const Tile& centerTile, int radius)
{
	const NAS2D::Point areaStartPoint
	{
		std::clamp(centerTile.xy().x - radius, 0, 299),
		std::clamp(centerTile.xy().y - radius, 0, 149)
	};

	const NAS2D::Point areaEndPoint
	{
		std::clamp(centerTile.xy().x + radius, 0, 299),
		std::clamp(centerTile.xy().y + radius, 0, 149)
	};

	return NAS2D::Rectangle<int>::Create(areaStartPoint, areaEndPoint);
}


static void pushAgingRobotMessage(const Robot* robot, const MapCoordinate& position, NotificationArea& notificationArea)
{
	const auto robotLocationText = "(" + std::to_string(position.xy.x) + ", " + std::to_string(position.xy.y) + ")";

	if (robot->fuelCellAge() == 190) /// \fixme magic number
	{
		notificationArea.push(
			"Aging Robot",
			"Robot '" + robot->name() + "' at location " + robotLocationText + " is approaching its maximum age.",
			position,
			NotificationArea::NotificationType::Warning);
	}
	else if (robot->fuelCellAge() == 195) /// \fixme magic number
	{
		notificationArea.push(
			"Aging Robot",
			"Robot '" + robot->name() + "' at location " + robotLocationText + " will fail in a few turns. Replace immediately.",
			position,
			NotificationArea::NotificationType::Critical);
	}
}


MapViewState::MapViewState(MainReportsUiState& mainReportsState, const std::string& savegame) :
	mMainReportsState(mainReportsState),
	mCrimeExecution(mNotificationArea),
	mLoadingExisting(true),
	mExistingToLoad(savegame),
	mResourceInfoBar{mResourcesCount, mPopulation, mCurrentMorale, mPreviousMorale, mFood},
	mRobotDeploymentSummary{mRobotPool}
{
	ccLocation() = CcNotPlaced;
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect(this, &MapViewState::onWindowResized);
}


MapViewState::MapViewState(MainReportsUiState& mainReportsState, const Planet::Attributes& planetAttributes, Difficulty selectedDifficulty) :
	mMainReportsState(mainReportsState),
	mTileMap(new TileMap(planetAttributes.mapImagePath, planetAttributes.tilesetPath, planetAttributes.maxDepth, planetAttributes.maxMines, planetAttributes.hostility)),
	mCrimeExecution(mNotificationArea),
	mPlanetAttributes(planetAttributes),
	mResourceInfoBar{mResourcesCount, mPopulation, mCurrentMorale, mPreviousMorale, mFood},
	mMiniMap{std::make_unique<MiniMap>(mTileMap, mRobotList, planetAttributes.mapImagePath)},
	mRobotDeploymentSummary{mRobotPool},
	mNavControl{std::make_unique<NavControl>(*mTileMap)}
{
	difficulty(selectedDifficulty);
	ccLocation() = CcNotPlaced;
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect(this, &MapViewState::onWindowResized);
}


MapViewState::~MapViewState()
{
	delete mPathSolver;

	scrubRobotList();
	delete mTileMap;

	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(PointerType::POINTER_NORMAL);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.activate().disconnect(this, &MapViewState::onActivate);
	eventHandler.keyDown().disconnect(this, &MapViewState::onKeyDown);
	eventHandler.mouseButtonDown().disconnect(this, &MapViewState::onMouseDown);
	eventHandler.mouseButtonUp().disconnect(this, &MapViewState::onMouseUp);
	eventHandler.mouseDoubleClick().disconnect(this, &MapViewState::onMouseDoubleClick);
	eventHandler.mouseMotion().disconnect(this, &MapViewState::onMouseMove);
	eventHandler.mouseWheel().disconnect(this, &MapViewState::onMouseWheel);
	eventHandler.windowResized().disconnect(this, &MapViewState::onWindowResized);

	eventHandler.textInputMode(false);

	NAS2D::Utility<std::map<class MineFacility*, Route>>::get().clear();
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
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.setCursor(PointerType::POINTER_NORMAL);

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

	setupUiPositions(renderer.size());
	resetPoliceOverlays();

	NAS2D::Utility<NAS2D::Renderer>::get().fadeIn(constants::FadeSpeed);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();

	eventHandler.activate().connect(this, &MapViewState::onActivate);
	eventHandler.keyDown().connect(this, &MapViewState::onKeyDown);
	eventHandler.mouseButtonDown().connect(this, &MapViewState::onMouseDown);
	eventHandler.mouseButtonUp().connect(this, &MapViewState::onMouseUp);
	eventHandler.mouseDoubleClick().connect(this, &MapViewState::onMouseDoubleClick);
	eventHandler.mouseMotion().connect(this, &MapViewState::onMouseMove);
	eventHandler.mouseWheel().connect(this, &MapViewState::onMouseWheel);

	eventHandler.textInputMode(true);

	MAIN_FONT = &fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);

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


void MapViewState::focusOnStructure(Structure* structure)
{
	if (!structure) { return; }
	mTileMap->centerOn(NAS2D::Utility<StructureManager>::get().tileFromStructure(structure).xyz());
}


void MapViewState::difficulty(Difficulty difficulty)
{
	mDifficulty = difficulty;
	mCrimeRateUpdate.difficulty(difficulty);
	mCrimeExecution.difficulty(difficulty);
}


/**
 * Updates the entire state of the game.
 */
NAS2D::State* MapViewState::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto renderArea = NAS2D::Rectangle<int>::Create({0, 0}, renderer.size());

	// Game's over, don't bother drawing anything else
	if (mGameOverDialog.visible())
	{
		renderer.drawBoxFilled(renderArea, NAS2D::Color::Black);
		mGameOverDialog.update();

		return this;
	}

	renderer.drawImageStretched(mBackground, renderArea);

	if (!modalUiElementDisplayed())
	{
		mTileMap->injectMouse(MOUSE_COORDS);
	}

	mTileMap->update();
	mTileMap->draw();

	// FIXME: Ugly / hacky
	if (modalUiElementDisplayed())
	{
		renderer.drawBoxFilled(renderArea, NAS2D::Color{0, 0, 0, 165});
	}

	drawUI();

	return this;
}


void MapViewState::updatePlayerResources()
{
	auto& storageTanks = NAS2D::Utility<StructureManager>::get().getStructures<StorageTanks>();
	auto& command = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();

	std::vector<Structure*> storage;
	storage.insert(storage.end(), command.begin(), command.end());
	storage.insert(storage.end(), storageTanks.begin(), storageTanks.end());

	StorableResources resources;
	for (auto structure : storage)
	{
		resources += structure->storage();
	}
	mResourcesCount = resources;
}


/**
 * Window activation handler.
 */
void MapViewState::onActivate(bool /*newActiveValue*/)
{
	mMiniMap->onActivate();
}


void MapViewState::onWindowResized(NAS2D::Vector<int> newSize)
{
	setupUiPositions(newSize);
	mTileMap->onResize(newSize);
}


/**
 * Key down event handler.
 */
void MapViewState::onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool /*repeat*/)
{
	if (!active()) { return; }

	// FIXME: Ugly / hacky
	if (modalUiElementDisplayed())
	{
		return;
	}

	if (key == NAS2D::EventHandler::KeyCode::KEY_F1)
	{
		mReportsUiSignal();
		return;
	}

	bool viewUpdated = false; // don't like flaggy code like this
	NAS2D::Point<int> pt = mTileMap->mapViewLocation();

	switch(key)
	{
		case NAS2D::EventHandler::KeyCode::KEY_w:
		case NAS2D::EventHandler::KeyCode::KEY_UP:
			viewUpdated = true;
			pt += DirectionNorth;
			break;

		case NAS2D::EventHandler::KeyCode::KEY_s:
		case NAS2D::EventHandler::KeyCode::KEY_DOWN:
			viewUpdated = true;
			pt += DirectionSouth;
			break;

		case NAS2D::EventHandler::KeyCode::KEY_a:
		case NAS2D::EventHandler::KeyCode::KEY_LEFT:
			viewUpdated = true;
			pt += DirectionWest;
			break;

		case NAS2D::EventHandler::KeyCode::KEY_d:
		case NAS2D::EventHandler::KeyCode::KEY_RIGHT:
			viewUpdated = true;
			pt += DirectionEast;
			break;

		case NAS2D::EventHandler::KeyCode::KEY_0:
			viewUpdated = true;
			changeViewDepth(0);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_1:
			viewUpdated = true;
			changeViewDepth(1);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_2:
			viewUpdated = true;
			changeViewDepth(2);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_3:
			viewUpdated = true;
			changeViewDepth(3);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_4:
			viewUpdated = true;
			changeViewDepth(4);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_PAGEUP:
			viewUpdated = true;
			changeViewDepth(mTileMap->currentDepth() - 1);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_PAGEDOWN:
			viewUpdated = true;
			changeViewDepth(mTileMap->currentDepth() + 1);
			break;


		case NAS2D::EventHandler::KeyCode::KEY_HOME:
			viewUpdated = true;
			changeViewDepth(0);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_END:
			viewUpdated = true;
			changeViewDepth(mTileMap->maxDepth());
			break;

		case NAS2D::EventHandler::KeyCode::KEY_F10:
			if (NAS2D::Utility<NAS2D::EventHandler>::get().control(mod) && NAS2D::Utility<NAS2D::EventHandler>::get().shift(mod))
			{
				StorableResources resourcesToAdd{1000, 1000, 1000, 1000};
				addRefinedResources(resourcesToAdd);
				updatePlayerResources();
				updateStructuresAvailability();
			}
			break;

		case NAS2D::EventHandler::KeyCode::KEY_F2:
			mFileIoDialog.scanDirectory(constants::SaveGamePath);
			mFileIoDialog.setMode(FileIo::FileOperation::Save);
			mFileIoDialog.show();
			break;

		case NAS2D::EventHandler::KeyCode::KEY_F3:
			mFileIoDialog.scanDirectory(constants::SaveGamePath);
			mFileIoDialog.setMode(FileIo::FileOperation::Load);
			mFileIoDialog.show();
			break;

		case NAS2D::EventHandler::KeyCode::KEY_ESCAPE:
			clearMode();
			resetUi();
			break;

		case NAS2D::EventHandler::KeyCode::KEY_ENTER:
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
void MapViewState::onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y)
{
	if (!active()) { return; }

	if (modalUiElementDisplayed()) { return; }

	if (mWindowStack.pointInWindow(MOUSE_COORDS))
	{
		mWindowStack.updateStack(MOUSE_COORDS);
		return;
	}

	if (button == NAS2D::EventHandler::MouseButton::Right || button == NAS2D::EventHandler::MouseButton::Middle)
	{
		if (mInsertMode != InsertMode::None)
		{
			resetUi();
			return;
		}

		if (!mTileMap->isMouseOverTile()) { return; }
		const auto tilePosition = mTileMap->mouseTilePosition();
		if (!mTileMap->isValidPosition(tilePosition)) { return; }

		const bool inspectModifier = NAS2D::Utility<NAS2D::EventHandler>::get().query_shift() ||
			button == NAS2D::EventHandler::MouseButton::Middle;

		onInspect(tilePosition, inspectModifier);
	}

	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		if (mTooltipSystemButton.rect().contains(MOUSE_COORDS))
		{
			onSystemMenu();
		}

		const auto oldDepth = mTileMap->currentDepth();
		mNavControl->onClick(MOUSE_COORDS);
		if (oldDepth != mTileMap->currentDepth())
		{
			changeViewDepth(mTileMap->currentDepth());
		}

		// MiniMap Check
		mMiniMap->onMouseDown(button, x, y);

		// Click was within the bounds of the TileMap.
		if (mTileMap->boundingBox().contains(MOUSE_COORDS))
		{
			auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
			onClickMap(eventHandler.query_shift());
		}
	}
}


void MapViewState::onMouseDoubleClick(NAS2D::EventHandler::MouseButton button, int /*x*/, int /*y*/)
{
	if (!active()) { return; }

	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		if (mWindowStack.pointInWindow(MOUSE_COORDS)) { return; }
		if (!mTileMap->isMouseOverTile()) { return; }
		const auto tilePosition = mTileMap->mouseTilePosition();
		if (!mTileMap->isValidPosition(tilePosition)) { return; }

		auto& tile = mTileMap->getTile(tilePosition);
		if (tile.thingIsStructure())
		{
			Structure* structure = tile.structure();

			if (structure->isFactory())
			{
				mMainReportsState.selectFactoryPanel(structure);
			}
			else if (structure->isWarehouse())
			{
				mMainReportsState.selectWarehousePanel(structure);
			}
			else if (structure->isMineFacility() || structure->structureClass() == Structure::StructureClass::Smelter)
			{
				mMainReportsState.selectMinePanel(structure);
			}
			else
			{
				// avoids showing the full-screen UI on unhandled structures.
				return;
			}

			mReportsUiSignal();
		}
	}
}


/**
* Mouse Up event handler.
*/
void MapViewState::onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y)
{
	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		mMiniMap->onMouseUp(button, x, y);
		auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
		if ((mInsertMode == InsertMode::Tube) && eventHandler.query_shift())
		{
			Tile* tile = mTileMap->getVisibleTile(mMouseTilePosition);
			if (!tile) { return; }

			placeTubeEnd(tile);
		}
	}
}


/**
* Mouse motion event handler.
*/
void MapViewState::onMouseMove(int x, int y, int rX, int rY)
{
	if (!active()) { return; }
	mMiniMap->onMouseMove(x, y, rX, rY);
	mMouseTilePosition = mTileMap->mouseTilePosition();
}


/**
 * Mouse wheel event handler.
 */
void MapViewState::onMouseWheel(int /*x*/, int y)
{
	if (mInsertMode != InsertMode::Tube) { return; }

	y > 0 ? mConnections.decrementSelection() : mConnections.incrementSelection();
}


void MapViewState::onInspect(const MapCoordinate& tilePosition, bool inspectModifier)
{
	auto& tile = mTileMap->getTile(tilePosition);
	if (tile.empty())
	{
		onInspectTile(tile);
	}
	else if (tile.thingIsRobot())
	{
		onInspectRobot(*tile.robot());
	}
	else if (tile.thingIsStructure())
	{
		onInspectStructure(*tile.structure(), inspectModifier);
	}
}


void MapViewState::onInspectStructure(Structure& structure, bool inspectModifier)
{
	const bool notDisabled = structure.operational() || structure.isIdle();
	const bool preferStructureSpecificView = notDisabled && !inspectModifier;

	if (structure.isFactory() && preferStructureSpecificView)
	{
		mFactoryProduction.factory(&static_cast<Factory&>(structure));
		mFactoryProduction.show();
		mWindowStack.bringToFront(&mFactoryProduction);
	}
	else if (structure.isWarehouse() && preferStructureSpecificView)
	{
		mWarehouseInspector.warehouse(&static_cast<Warehouse&>(structure));
		mWarehouseInspector.show();
		mWindowStack.bringToFront(&mWarehouseInspector);
	}
	else if (structure.isMineFacility() && preferStructureSpecificView)
	{
		mMineOperationsWindow.mineFacility(&static_cast<MineFacility&>(structure));
		mMineOperationsWindow.show();
		mWindowStack.bringToFront(&mMineOperationsWindow);
	}
	else
	{
		mStructureInspector.structure(&structure);
		mStructureInspector.show();
		mWindowStack.bringToFront(&mStructureInspector);
	}
}


void MapViewState::onInspectRobot(Robot& robot)
{
	mRobotInspector.focusOnRobot(&robot);
	mRobotInspector.show();
	mWindowStack.bringToFront(&mRobotInspector);
}


void MapViewState::onInspectTile(Tile& tile)
{
	clearSelections();
	mTileInspector.tile(&tile);
	mTileInspector.show();
	mWindowStack.bringToFront(&mTileInspector);
}


void MapViewState::onClickMap(bool isShiftPressed)
{
	Tile* tile = mTileMap->getVisibleTile();
	if (!tile) { return; }

	if (mInsertMode == InsertMode::Structure)
	{
		placeStructure(tile);
	}
	else if (mInsertMode == InsertMode::Robot)
	{
		placeRobot(tile);
	}
	else if ((mInsertMode == InsertMode::Tube) && isShiftPressed)
	{
		placeTubeStart(tile);
	}
	else if (mInsertMode == InsertMode::Tube)
	{
		placeTubes(tile);
	}
}


void MapViewState::onSystemMenu()
{
	mGameOptionsDialog.show();
	resetUi();
}


/**
 * Changes the current view depth.
 */
void MapViewState::changeViewDepth(int depth)
{
	if (mBtnTogglePoliceOverlay.toggled())
	{
		changePoliceOverlayDepth(mTileMap->currentDepth(), depth);
	}

	mTileMap->currentDepth(depth);

	if (mInsertMode != InsertMode::Robot) { clearMode(); }
	populateStructureMenu();
}


/**
 * Clears the build mode.
 */
void MapViewState::clearMode()
{
	mInsertMode = InsertMode::None;
	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(PointerType::POINTER_NORMAL);

	mCurrentStructure = StructureID::SID_NONE;
	mCurrentRobot = Robot::Type::None;

	clearSelections();
}


void MapViewState::insertTube(ConnectorDir dir, int depth, Tile* tile)
{
	if (dir == ConnectorDir::CONNECTOR_VERTICAL)
	{
		throw std::runtime_error("MapViewState::insertTube() called with invalid ConnectorDir paramter.");
	}

	NAS2D::Utility<StructureManager>::get().addStructure(new Tube(dir, depth != 0), tile);
}


void MapViewState::placeTubes(Tile* tile)
{
	// Check the basics.
	if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated()) { return; }

	/** \fixme	This is a kludge that only works because all of the tube structures are listed alphabetically.
	 *			Should instead take advantage of the updated meta data in the IconGridItem.
	 */
	auto cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);

	if (validTubeConnection(*mTileMap, mMouseTilePosition, cd))
	{
		insertTube(cd, mTileMap->currentDepth(), &mTileMap->getTile(mMouseTilePosition));

		// FIXME: Naive approach -- will be slow with larger colonies.
		NAS2D::Utility<StructureManager>::get().disconnectAll();
		checkConnectedness();
	}
	else
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertTubeInvalidLocation);
	}
}

void MapViewState::placeTubeStart(Tile* tile)
{
	mPlacingTube = false;

	// Check the basics.
	if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated()) { return; }

	/** \fixme	This is a kludge that only works because all of the tube structures are listed alphabetically.
	 *			Should instead take advantage of the updated meta data in the IconGridItem.
	 */
	ConnectorDir cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);

	if (!validTubeConnection(*mTileMap, mMouseTilePosition, cd))
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertTubeInvalidLocation);
		return;
	}
	mTubeStart = tile->xy();
	mPlacingTube = true;
}


void MapViewState::placeTubeEnd(Tile* tile)
{
	if (!mPlacingTube) return;
	mPlacingTube = false;

	/** \fixme	This is a kludge that only works because all of the tube structures are listed alphabetically.
	 *			Should instead take advantage of the updated meta data in the IconGridItem.
	 */
	ConnectorDir cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);

	const auto startEndDirection = tile->xy() - mTubeStart;
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
		tile = mTileMap->getVisibleTile({mTubeStart, mTileMap->currentDepth()});
		if (!tile) {
			endReach = true;
		}else if (tile->thing() || tile->mine() || !tile->bulldozed() || !tile->excavated()){
			endReach = true;
		}else if (!validTubeConnection(*mTileMap, {position, mTileMap->currentDepth()}, cd)){
			endReach = true;
		}else{
			insertTube(cd, mTileMap->currentDepth(), &mTileMap->getTile(position));

			// FIXME: Naive approach -- will be slow with larger colonies.
			NAS2D::Utility<StructureManager>::get().disconnectAll();
			checkConnectedness();
		}

		if (position == tubeEnd) endReach = true;
		position += tubeDirection;
	} while (!endReach);
}


void MapViewState::placeRobodozer(Tile& tile)
{
	auto& robot = mRobotPool.getDozer();

	if (tile.thing() && !tile.thingIsStructure())
	{
		return;
	}
	else if (tile.index() == TerrainType::Dozed && !tile.thingIsStructure())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertTileBulldozed);
		return;
	}
	else if (tile.mine())
	{
		if (tile.mine()->depth() != mTileMap->maxDepth() || !tile.mine()->exhausted())
		{
			doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMineNotExhausted);
			return;
		}

		mMineOperationsWindow.hide();
		const auto tilePosition = mTileMap->mouseTilePosition().xy;
		mTileMap->removeMineLocation(tilePosition);
		tile.pushMine(nullptr);
		for (int i = 0; i <= mTileMap->maxDepth(); ++i)
		{
			auto& mineShaftTile = mTileMap->getTile({tilePosition, i});
			NAS2D::Utility<StructureManager>::get().removeStructure(mineShaftTile.structure());
		}
	}
	else if (tile.thingIsStructure())
	{
		if (mStructureInspector.structure() == tile.structure()) { mStructureInspector.hide(); }

		Structure* structure = tile.structure();

		if (structure->isMineFacility()) { return; }
		if (structure->structureClass() == Structure::StructureClass::Command)
		{
			doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertCannotBulldozeCc);
			return;
		}

		if (structure->structureClass() == Structure::StructureClass::Lander && structure->age() == 0)
		{
			doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertCannotBulldozeLandingSite);
			return;
		}

		if (structure->isRobotCommand())
		{
			auto* rcc = static_cast<RobotCommand*>(structure);
			if (rcc->isControlling(&robot))
			{
				mNotificationArea.push(
					"Cannot bulldoze",
					"Cannot bulldoze Robot Command Center by a Robot under its command.",
					tile.xyz(),
					NotificationArea::NotificationType::Information);
			}
			else
			{
				deleteRobotsInRCC(rcc, mRobotPool, mRobotList);
			}
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

		if (structure->structureClass() == Structure::StructureClass::Communication)
		{
			checkCommRangeOverlay();
		}

		auto recycledResources = StructureCatalogue::recyclingValue(structure->structureId());
		addRefinedResources(recycledResources);

		/**
		 * \todo	This could/should be some sort of alert message to the user instead of dumped to the console
		 */
		if (!recycledResources.isEmpty())
		{
			mNotificationArea.push(
				"Resources wasted",
				"Resources wasted demolishing " + structure->name(),
				tile.xyz(),
				NotificationArea::NotificationType::Warning);
		}

		updatePlayerResources();
		updateStructuresAvailability();

		tile.connected(false);
		NAS2D::Utility<StructureManager>::get().removeStructure(structure);
		tile.deleteThing();
		NAS2D::Utility<StructureManager>::get().disconnectAll();
		robot.tileIndex(static_cast<std::size_t>(TerrainType::Dozed));
		checkConnectedness();
	}

	int taskTime = tile.index() == TerrainType::Dozed ? 1 : static_cast<int>(tile.index());
	robot.startTask(taskTime);
	mRobotPool.insertRobotIntoTable(mRobotList, &robot, &tile);
	robot.tileIndex(static_cast<std::size_t>(tile.index()));
	tile.index(TerrainType::Dozed);

	if (!mRobotPool.robotAvailable(Robot::Type::Dozer))
	{
		mRobots.removeItem(constants::Robodozer);
		clearMode();
	}
}


void MapViewState::placeRobodigger(Tile& tile)
{
	// Keep digger within a safe margin of the map boundaries.
	if (!NAS2D::Rectangle<int>::Create({4, 4}, NAS2D::Point{-4, -4} + mTileMap->size()).contains(mMouseTilePosition.xy))
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertDiggerEdgeBuffer);
		return;
	}

	// Check for obstructions underneath the the digger location.
	if (tile.depth() != mTileMap->maxDepth() && !mTileMap->getTile({tile.xy(), tile.depth() + 1}).empty())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertDiggerBlockedBelow);
		return;
	}

	if (tile.hasMine())
	{
		if (!doYesNoMessage(constants::AlertDiggerMineTile, constants::AlertDiggerMine)) { return; }

		const auto position = tile.xy();
		mNotificationArea.push(
			"Mine destroyed",
			"Digger destroyed a Mine at (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ").",
			tile.xyz(),
			NotificationArea::NotificationType::Information);
		mTileMap->removeMineLocation(position);
	}

	// Die if tile is occupied or not excavated.
	if (!tile.empty())
	{
		if (tile.depth() > constants::DepthSurface)
		{
			if (tile.thingIsStructure() && tile.structure()->connectorDirection() != ConnectorDir::CONNECTOR_VERTICAL) //air shaft
			{
				doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertStructureInWay);
				return;
			}
			else if (tile.thingIsStructure() && tile.structure()->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL && tile.depth() == mTileMap->maxDepth())
			{
				doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMaxDigDepth);
				return;
			}
		}
		else
		{
			doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertStructureInWay);
			return;
		}
	}

	if (!tile.thing() && mTileMap->currentDepth() > 0) { mDiggerDirection.cardinalOnlyEnabled(); }
	else { mDiggerDirection.downOnlyEnabled(); }

	mDiggerDirection.setParameters(&tile);

	// If we're placing on the top level we can only ever go down.
	if (mTileMap->currentDepth() == constants::DepthSurface)
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
		if (position.x + mDiggerDirection.size().x > NAS2D::Utility<NAS2D::Renderer>::get().size().x)
		{
			// Popup to the left of the mouse
			position = MOUSE_COORDS + NAS2D::Vector{-20 - mDiggerDirection.size().x, -32};
		}
		mDiggerDirection.position(position);
	}
}


void MapViewState::placeRobominer(Tile& tile)
{
	if (tile.thing())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMinerTileObstructed);
		return;
	}
	if (mTileMap->currentDepth() != constants::DepthSurface)
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMinerSurfaceOnly);
		return;
	}
	if (!tile.mine())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMinerNotOnMine);
		return;
	}

	auto& robot = mRobotPool.getMiner();
	robot.startTask(constants::MinerTaskTime);
	mRobotPool.insertRobotIntoTable(mRobotList, &robot, &tile);
	tile.index(TerrainType::Dozed);

	if (!mRobotPool.robotAvailable(Robot::Type::Miner))
	{
		mRobots.removeItem(constants::Robominer);
		clearMode();
	}
}


void MapViewState::placeRobot(Tile* tile)
{
	if (!tile->excavated()) { return; }
	if (!mRobotPool.robotCtrlAvailable()) { return; }

	if (!inCommRange(tile->xy()))
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertOutOfCommRange);
		return;
	}

	switch (mCurrentRobot)
	{
	case Robot::Type::Dozer:
		placeRobodozer(*tile);
		break;
	case Robot::Type::Digger:
		placeRobodigger(*tile);
		break;
	case Robot::Type::Miner:
		placeRobominer(*tile);
		break;
	default:
		break;
	}
}


/**
 * Checks the robot selection interface and if the robot is not available in it, adds
 * it back in.
 */
void MapViewState::checkRobotSelectionInterface(Robot::Type rType)
{
	const auto& robotInfo = RobotMetaTable.at(rType);
	if (!mRobots.itemExists(robotInfo.name))
	{
		mRobots.addItemSorted(robotInfo.name, robotInfo.sheetIndex, static_cast<int>(rType));
	}
}


/**
 * Places a structure into the map.
 */
void MapViewState::placeStructure(Tile* tile)
{
	if (mCurrentStructure == StructureID::SID_NONE) { throw std::runtime_error("MapViewState::placeStructure() called but mCurrentStructure == STRUCTURE_NONE"); }

	if (!structureIsLander(mCurrentStructure) && !selfSustained(mCurrentStructure) &&
		!isPointInRange(tile->xy(), ccLocation(), constants::RobotCommRange))
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureOutOfRange);
		return;
	}

	if (tile->mine())
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureMineInWay);
		return;
	}

	if (tile->thing())
	{
		if (tile->thingIsStructure())
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTileObstructed);
		}
		else
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTileThing);
		}
		return;
	}

	if ((!tile->bulldozed() && !structureIsLander(mCurrentStructure)))
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTerrain);
		return;
	}

	if (!tile->excavated())
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureExcavated);
		return;
	}

	// The player may only place one seed lander per game.
	if (mCurrentStructure == StructureID::SID_SEED_LANDER)
	{
		insertSeedLander(mMouseTilePosition.xy);
	}
	else if (mCurrentStructure == StructureID::SID_COLONIST_LANDER)
	{
		if (!validLanderSite(*tile)) { return; }

		ColonistLander* s = new ColonistLander(tile);
		s->deploySignal().connect(this, &MapViewState::onDeployColonistLander);
		NAS2D::Utility<StructureManager>::get().addStructure(s, tile);

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

		CargoLander* cargoLander = new CargoLander(tile);
		cargoLander->deploySignal().connect(this, &MapViewState::onDeployCargoLander);
		NAS2D::Utility<StructureManager>::get().addStructure(cargoLander, tile);

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
		if (!validStructurePlacement(*mTileMap, mMouseTilePosition) && !selfSustained(mCurrentStructure))
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureNoTube);
			return;
		}

		// Check build cost
		if (!StructureCatalogue::canBuild(mResourcesCount, mCurrentStructure))
		{
			resourceShortageMessage(mResourcesCount, mCurrentStructure);
			return;
		}

		Structure* structure = StructureCatalogue::get(mCurrentStructure);
		if (!structure) { throw std::runtime_error("MapViewState::placeStructure(): NULL Structure returned from StructureCatalog."); }

		NAS2D::Utility<StructureManager>::get().addStructure(structure, tile);

		// FIXME: Ugly
		if (structure->isFactory())
		{
			static_cast<Factory*>(structure)->productionComplete().connect(this, &MapViewState::onFactoryProductionComplete);
			static_cast<Factory*>(structure)->resourcePool(&mResourcesCount);
		}

		if (structure->structureId() == StructureID::SID_MAINTENANCE_FACILITY)
		{
			static_cast<MaintenanceFacility*>(structure)->resources(mResourcesCount);
		}

		auto cost = StructureCatalogue::costToBuild(mCurrentStructure);
		removeRefinedResources(cost);
		updatePlayerResources();
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
		if (!landingSiteSuitable(*mTileMap, point))
		{
			return;
		}

		SeedLander* s = new SeedLander(point);
		s->deploySignal().connect(this, &MapViewState::onDeploySeedLander);
		NAS2D::Utility<StructureManager>::get().addStructure(s, &mTileMap->getTile(point)); // Can only ever be placed on depth level 0

		clearMode();
		resetUi();

		mStructures.clear();
		mBtnTurns.enabled(true);
	}
	else
	{
		doAlertMessage(constants::AlertLanderLocation, constants::AlertSeedEdgeBuffer);
	}
}


/**
 * Updates all robots.
 */
void MapViewState::updateRobots()
{
	auto robot_it = mRobotList.begin();
	while (robot_it != mRobotList.end())
	{
		auto robot = robot_it->first;
		auto tile = robot_it->second;

		robot->update();

		const auto position = tile->xyz();

		pushAgingRobotMessage(robot, position, mNotificationArea);

		if (robot->dead())
		{
			const auto robotLocationText = "(" +  std::to_string(position.xy.x) + ", " + std::to_string(position.xy.y) + ")";

			if (robot->selfDestruct())
			{
				mNotificationArea.push(
					"Robot Self-Destructed",
					robot->name() + " at location " + robotLocationText + " self destructed.",
					position,
					NotificationArea::NotificationType::Critical);
			}
			else if (robot->type() != Robot::Type::Miner)
			{
				const auto text = "Your " + robot->name() + " at location " + robotLocationText + " has broken down. It will not be able to complete its task and will be removed from your inventory.";
				mNotificationArea.push("Robot Broke Down", text, position, NotificationArea::NotificationType::Critical);
				resetTileIndexFromDozer(robot, tile);
			}

			if (tile->thing() == robot)
			{
				tile->removeThing();
			}

			for (auto rcc : NAS2D::Utility<StructureManager>::get().getStructures<RobotCommand>())
			{
				rcc->removeRobot(robot);
			}

			if (mRobotInspector.focusedRobot() == robot) { mRobotInspector.hide(); }

			mRobotPool.erase(robot);
			delete robot;
			robot_it = mRobotList.erase(robot_it);
		}
		else if (robot->idle())
		{
			if (tile->thing() == robot)
			{
				tile->removeThing();
			}
			robot_it = mRobotList.erase(robot_it);

			if (robot->taskCanceled())
			{
				resetTileIndexFromDozer(robot, tile);
				checkRobotSelectionInterface(robot->type());
				robot->reset();
			}
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
	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(PointerType::POINTER_PLACE_TILE);
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
	auto& tile = mTileMap->getTile({ccLocation(), 0});
	Structure* cc = tile.structure();

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
	mConnectednessOverlay.clear();
	GraphWalker graphWalker({ccLocation(), 0}, *mTileMap, mConnectednessOverlay);
}


void MapViewState::checkCommRangeOverlay()
{
	mCommRangeOverlay.clear();

	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	const auto& commTowers = structureManager.getStructures<CommTower>();
	const auto& command = structureManager.getStructures<CommandCenter>();

	for (auto cc : command)
	{
		if (!cc->operational()) { continue; }
		auto& centerTile = structureManager.tileFromStructure(cc);
		fillRangedAreaList(mCommRangeOverlay, centerTile, cc->getRange());
	}

	for (auto tower : commTowers)
	{
		if (!tower->operational()) { continue; }
		auto& centerTile = structureManager.tileFromStructure(tower);
		fillRangedAreaList(mCommRangeOverlay, centerTile, tower->getRange());
	}
}


void MapViewState::checkSurfacePoliceOverlay()
{
	resetPoliceOverlays();

	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	const auto& policeStations = structureManager.getStructures<SurfacePolice>();

	for (auto policeStation : policeStations)
	{
		if (!policeStation->operational()) { continue; }
		auto& centerTile = structureManager.tileFromStructure(policeStation);
		fillRangedAreaList(mPoliceOverlays[0], centerTile, policeStation->getRange());
	}

	const auto& undergroundPoliceStations = structureManager.getStructures<UndergroundPolice>();

	for (auto undergroundPoliceStation : undergroundPoliceStations)
	{
		if (!undergroundPoliceStation->operational()) { continue; }
		auto depth = structureManager.tileFromStructure(undergroundPoliceStation).depth();
		auto& centerTile = structureManager.tileFromStructure(undergroundPoliceStation);
		fillRangedAreaList(mPoliceOverlays[depth], centerTile, undergroundPoliceStation->getRange(), depth);
	}
}


void MapViewState::resetPoliceOverlays()
{
	mPoliceOverlays.clear();
	for (int i = 0; i <= mTileMap->maxDepth(); ++i)
	{
		mPoliceOverlays.push_back(std::vector<Tile*>());
	}
}


void MapViewState::fillRangedAreaList(std::vector<Tile*>& tileList, Tile& centerTile, int range)
{
	fillRangedAreaList(tileList, centerTile, range, 0);
}

void MapViewState::fillRangedAreaList(std::vector<Tile*>& tileList, Tile& centerTile, int range, int depth)
{
	auto area = buildAreaRectFromTile(centerTile, range + 1);

	for (int y = 0; y < area.height; ++y)
	{
		for (int x = 0; x < area.width; ++x)
		{
			auto& tile = (*mTileMap).getTile({{x + area.x, y + area.y}, depth});
			if (isPointInRange(centerTile.xy(), tile.xy(), range))
			{
				if (std::find(tileList.begin(), tileList.end(), &tile) == tileList.end())
				{
					tileList.push_back(&tile);
				}
			}
		}
	}
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
