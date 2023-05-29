#include "MapViewState.h"

#include "MainMenuState.h"
#include "MainReportsUiState.h"
#include "Route.h"

#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include "../DirectionOffset.h"
#include "../Cache.h"
#include "../ProductCatalogue.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"

#include "../Map/Tile.h"
#include "../Map/TileMap.h"
#include "../Map/MapView.h"

#include "../UI/MessageBox.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/PointInRectangleRange.h>

#include <algorithm>
#include <sstream>
#include <vector>


extern NAS2D::Point<int> MOUSE_COORDS;

const NAS2D::Font* MAIN_FONT = nullptr;


namespace
{
	// Relative proportion of mines with yields {low, med, high}
	const std::map<Planet::Hostility, std::array<int, 3>> HostilityMineYields =
	{
		{Planet::Hostility::Low, {30, 50, 20}},
		{Planet::Hostility::Medium, {45, 35, 20}},
		{Planet::Hostility::High, {35, 20, 45}},
	};

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


	NAS2D::Point<int> clampPointToRect(NAS2D::Point<int> point, const NAS2D::Rectangle<int>& rect)
	{
		const auto endPoint = rect.endPoint();
		return {
			std::clamp(point.x, rect.position.x, endPoint.x),
			std::clamp(point.y, rect.position.y, endPoint.y),
		};
	}


	NAS2D::Rectangle<int> buildTileRectFromCenter(const NAS2D::Point<int>& centerPoint, int radius)
	{
		const auto mapRect = NAS2D::Rectangle<int>{{0, 0}, {299, 149}};
		const auto offset = NAS2D::Vector{radius, radius};
		const auto areaStartPoint = clampPointToRect(centerPoint - offset, mapRect);
		const auto areaEndPoint = clampPointToRect(centerPoint + offset + NAS2D::Vector{1, 1}, mapRect);
		return NAS2D::Rectangle<int>::Create(areaStartPoint, areaEndPoint);
	}


	void fillOverlayCircle(TileMap& tileMap, std::vector<Tile*>& tileList, Tile& centerTile, int range)
	{
		const auto center = centerTile.xy();
		const auto depth = centerTile.depth();
		auto tileRect = buildTileRectFromCenter(center, range);

		for (const auto point : NAS2D::PointInRectangleRange(tileRect))
		{
			if (isPointInRange(center, point, range))
			{
				auto& tile = tileMap.getTile({point, depth});
				if (std::find(tileList.begin(), tileList.end(), &tile) == tileList.end())
				{
					tileList.push_back(&tile);
				}
			}
		}
	}


	template <typename StructureType>
	void fillOverlay(TileMap& tileMap, std::vector<Tile*>& overlay, const std::vector<StructureType*> structures)
	{
		auto& structureManager = NAS2D::Utility<StructureManager>::get();
		for (auto structure : structures)
		{
			if (!structure->operational()) { continue; }
			auto& centerTile = structureManager.tileFromStructure(structure);
			fillOverlayCircle(tileMap, overlay, centerTile, structure->getRange());
		}
	}


	template <typename StructureType>
	void fillOverlay(TileMap& tileMap, std::vector<std::vector<Tile*>>& overlays, const std::vector<StructureType*> structures)
	{
		auto& structureManager = NAS2D::Utility<StructureManager>::get();
		for (auto structure : structures)
		{
			if (!structure->operational()) { continue; }
			auto& centerTile = structureManager.tileFromStructure(structure);
			fillOverlayCircle(tileMap, overlays[static_cast<std::size_t>(centerTile.depth())], centerTile, structure->getRange());
		}
	}


	void pushAgingRobotMessage(const Robot* robot, const MapCoordinate& position, NotificationArea& notificationArea)
	{
		const auto robotLocationText = "(" + std::to_string(position.xy.x) + ", " + std::to_string(position.xy.y) + ")";

		if (robot->fuelCellAge() == 190) // FIXME: magic number
		{
			notificationArea.push({
				"Aging Robot",
				"Robot '" + robot->name() + "' at location " + robotLocationText + " is approaching its maximum age.",
				position,
				NotificationArea::NotificationType::Warning});
		}
		else if (robot->fuelCellAge() == 195) // FIXME: magic number
		{
			notificationArea.push({
				"Aging Robot",
				"Robot '" + robot->name() + "' at location " + robotLocationText + " will fail in a few turns. Replace immediately.",
				position,
				NotificationArea::NotificationType::Critical});
		}
	}


	void updateFade(NAS2D::Renderer& renderer, NAS2D::Fade& fade)
	{
		fade.update();
		fade.draw(renderer);
	}
}


MapViewState::MapViewState(MainReportsUiState& mainReportsState, const std::string& savegame) :
	mCrimeExecution(mNotificationArea),
	mTechnologyReader("tech0-1.xml"),
	mLoadingExisting(true),
	mExistingToLoad(savegame),
	mMainReportsState(mainReportsState),
	mResourceInfoBar{mResourcesCount, mPopulation, mCurrentMorale, mPreviousMorale, mFood},
	mRobotDeploymentSummary{mRobotPool}
{
	ccLocation() = CcNotPlaced;
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect({this, &MapViewState::onWindowResized});
}


MapViewState::MapViewState(MainReportsUiState& mainReportsState, const Planet::Attributes& planetAttributes, Difficulty selectedDifficulty) :
	mTileMap(new TileMap(planetAttributes.mapImagePath, planetAttributes.maxDepth, planetAttributes.maxMines, HostilityMineYields.at(planetAttributes.hostility))),
	mCrimeExecution(mNotificationArea),
	mTechnologyReader("tech0-1.xml"),
	mPlanetAttributes(planetAttributes),
	mMainReportsState(mainReportsState),
	mMapView{std::make_unique<MapView>(*mTileMap)},
	mResourceInfoBar{mResourcesCount, mPopulation, mCurrentMorale, mPreviousMorale, mFood},
	mRobotDeploymentSummary{mRobotPool},
	mMiniMap{std::make_unique<MiniMap>(*mMapView, mTileMap, mRobotList, planetAttributes.mapImagePath)},
	mDetailMap{std::make_unique<DetailMap>(*mMapView, *mTileMap, planetAttributes.tilesetPath)},
	mNavControl{std::make_unique<NavControl>(*mMapView, *mTileMap)}
{
	setMeanSolarDistance(mPlanetAttributes.meanSolarDistance);
	difficulty(selectedDifficulty);
	ccLocation() = CcNotPlaced;
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect({this, &MapViewState::onWindowResized});
}


MapViewState::~MapViewState()
{
	delete mPathSolver;

	scrubRobotList();
	delete mTileMap;

	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(PointerType::POINTER_NORMAL);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.activate().disconnect({this, &MapViewState::onActivate});
	eventHandler.keyDown().disconnect({this, &MapViewState::onKeyDown});
	eventHandler.mouseButtonDown().disconnect({this, &MapViewState::onMouseDown});
	eventHandler.mouseButtonUp().disconnect({this, &MapViewState::onMouseUp});
	eventHandler.mouseDoubleClick().disconnect({this, &MapViewState::onMouseDoubleClick});
	eventHandler.mouseMotion().disconnect({this, &MapViewState::onMouseMove});
	eventHandler.mouseWheel().disconnect({this, &MapViewState::onMouseWheel});
	eventHandler.windowResized().disconnect({this, &MapViewState::onWindowResized});

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

	StructureCatalogue::init();
	ProductCatalogue::init("factory_products.xml");

	if (mLoadingExisting)
	{
		load(mExistingToLoad);
	}

	mResourceInfoBar.ignoreGlow(mTurnCount == 0);

	setupUiPositions(renderer.size());
	resetPoliceOverlays();
    
    mMainReportsState.injectTechnology(mTechnologyReader, mResearchTracker);

	mFade.fadeIn(constants::FadeSpeed);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();

	eventHandler.activate().connect({this, &MapViewState::onActivate});
	eventHandler.keyDown().connect({this, &MapViewState::onKeyDown});
	eventHandler.mouseButtonDown().connect({this, &MapViewState::onMouseDown});
	eventHandler.mouseButtonUp().connect({this, &MapViewState::onMouseUp});
	eventHandler.mouseDoubleClick().connect({this, &MapViewState::onMouseDoubleClick});
	eventHandler.mouseMotion().connect({this, &MapViewState::onMouseMove});
	eventHandler.mouseWheel().connect({this, &MapViewState::onMouseWheel});

	eventHandler.textInputMode(true);

	MAIN_FONT = &fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal);

	delete mPathSolver;
	mPathSolver = new micropather::MicroPather(mTileMap, 250, 6, false);
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
	onTakeMeThere(NAS2D::Utility<StructureManager>::get().tileFromStructure(structure).xyz());
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
	const auto windowClientRect = NAS2D::Rectangle{{0, 0}, renderer.size()};

	// Game's over, don't bother drawing anything else
	if (mGameOverDialog.visible())
	{
		renderer.drawBoxFilled(windowClientRect, NAS2D::Color::Black);
		mGameOverDialog.update();

		updateFade(renderer, mFade);

		return this;
	}

	renderer.drawImageStretched(mBackground, windowClientRect);

	if (!modalUiElementDisplayed())
	{
		mDetailMap->onMouseMove(MOUSE_COORDS);
	}

	mDetailMap->update();
	mDetailMap->draw();

	// FIXME: Ugly / hacky
	if (modalUiElementDisplayed())
	{
		renderer.drawBoxFilled(windowClientRect, NAS2D::Color{0, 0, 0, 165});
	}

	drawUI();

	updateFade(renderer, mFade);

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
	mDetailMap->resize(newSize);
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

	if (mCheatMenu.visible() && mCheatMenu.hasFocus())
	{
		return;
	}

	if (key == NAS2D::EventHandler::KeyCode::KEY_F1)
	{
		mReportsUiSignal();
		return;
	}

	switch(key)
	{
		case NAS2D::EventHandler::KeyCode::KEY_w:
		case NAS2D::EventHandler::KeyCode::KEY_UP:
			mMapView->moveView(MapOffsetNorthWest);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_s:
		case NAS2D::EventHandler::KeyCode::KEY_DOWN:
			mMapView->moveView(MapOffsetSouthEast);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_a:
		case NAS2D::EventHandler::KeyCode::KEY_LEFT:
			mMapView->moveView(MapOffsetSouthWest);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_d:
		case NAS2D::EventHandler::KeyCode::KEY_RIGHT:
			mMapView->moveView(MapOffsetNorthEast);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_0:
			changeViewDepth(0);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_1:
			changeViewDepth(1);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_2:
			changeViewDepth(2);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_3:
			changeViewDepth(3);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_4:
			changeViewDepth(4);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_PAGEUP:
			mMapView->moveView(MapOffsetUp);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_PAGEDOWN:
			mMapView->moveView(MapOffsetDown);
			break;


		case NAS2D::EventHandler::KeyCode::KEY_HOME:
			changeViewDepth(0);
			break;

		case NAS2D::EventHandler::KeyCode::KEY_END:
			changeViewDepth(mTileMap->maxDepth());
			break;

		case NAS2D::EventHandler::KeyCode::KEY_F10:
			if (NAS2D::Utility<NAS2D::EventHandler>::get().control(mod) && NAS2D::Utility<NAS2D::EventHandler>::get().shift(mod))
			{
				mCheatMenu.show();
				mWindowStack.bringToFront(&mCheatMenu);
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
}


void MapViewState::onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
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

		if (!mDetailMap->isMouseOverTile()) { return; }
		const auto tilePosition = mDetailMap->mouseTilePosition();
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

		const auto oldDepth = mMapView->currentDepth();
		mNavControl->onClick(MOUSE_COORDS);
		if (oldDepth != mMapView->currentDepth())
		{
			changeViewDepth(mMapView->currentDepth());
		}

		// MiniMap Check
		mMiniMap->onMouseDown(button, position);

		// Click was within the bounds of the TileMap.
		if (mDetailMap->isMouseOverTile())
		{
			onClickMap();
		}
	}
}


void MapViewState::onMouseDoubleClick(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> /*position*/)
{
	if (!active()) { return; }

	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		if (mWindowStack.pointInWindow(MOUSE_COORDS)) { return; }
		if (!mDetailMap->isMouseOverTile()) { return; }
		const auto tilePosition = mDetailMap->mouseTilePosition();
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
			else if (structure->isMineFacility() || structure->isSmelter())
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


void MapViewState::onMouseUp(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (button == NAS2D::EventHandler::MouseButton::Left)
	{
		mMiniMap->onMouseUp(button, position);
	}
}


void MapViewState::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative)
{
	if (!active()) { return; }
	mMiniMap->onMouseMove(position, relative);
	mMouseTilePosition = mDetailMap->mouseTilePosition();
}


void MapViewState::onMouseWheel(NAS2D::Vector<int> changeAmount)
{
	if (mInsertMode != InsertMode::Tube) { return; }

	changeAmount.y > 0 ? mConnections.decrementSelection() : mConnections.incrementSelection();
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
	mTileInspector.tile(tile);
	mTileInspector.show();
	mWindowStack.bringToFront(&mTileInspector);
}


void MapViewState::onClickMap()
{
	if (!mDetailMap->isMouseOverTile()) { return; }
	Tile& tile = mDetailMap->mouseTile();

	if (mInsertMode == InsertMode::Structure)
	{
		placeStructure(tile);
	}
	else if (mInsertMode == InsertMode::Robot)
	{
		placeRobot(tile);
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
	if (mBtnTogglePoliceOverlay.isPressed())
	{
		changePoliceOverlayDepth(mMapView->currentDepth(), depth);
	}

	mMapView->currentDepth(depth);

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


void MapViewState::insertTube(ConnectorDir dir, int depth, Tile& tile)
{
	if (dir == ConnectorDir::CONNECTOR_VERTICAL)
	{
		throw std::runtime_error("MapViewState::insertTube() called with invalid ConnectorDir paramter.");
	}

	NAS2D::Utility<StructureManager>::get().addStructure(*new Tube(dir, depth != 0), tile);
}


void MapViewState::placeTubes(Tile& tile)
{
	if (!tile.bulldozed()) {
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertTubeTerrain);
		return;
	}

	if (tile.thing() || tile.mine() || !tile.excavated()) { return; }

	/** FIXME: This is a kludge that only works because all of the tube structures are listed alphabetically.
	 * Should instead take advantage of the updated meta data in the IconGrid::Item.
	 */
	auto cd = static_cast<ConnectorDir>(mConnections.selectionIndex() + 1);

	if (validTubeConnection(*mTileMap, mMouseTilePosition, cd))
	{
		insertTube(cd, mMapView->currentDepth(), mTileMap->getTile(mMouseTilePosition));

		// FIXME: Naive approach -- will be slow with larger colonies.
		updateConnectedness();
	}
	else
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertTubeInvalidLocation);
	}
}


/**
 * Places a structure into the map.
 */
void MapViewState::placeStructure(Tile& tile)
{
	if (mCurrentStructure == StructureID::SID_NONE) { throw std::runtime_error("MapViewState::placeStructure() called but mCurrentStructure == STRUCTURE_NONE"); }

	if (!structureIsLander(mCurrentStructure) && !selfSustained(mCurrentStructure) &&
		!isPointInRange(tile.xy(), ccLocation(), constants::RobotCommRange))
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureOutOfRange);
		return;
	}

	if (tile.mine())
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureMineInWay);
		return;
	}

	if (tile.thing())
	{
		if (tile.thingIsStructure())
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTileObstructed);
		}
		else
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTileMapObject);
		}
		return;
	}

	if ((!tile.bulldozed() && !structureIsLander(mCurrentStructure)))
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTerrain);
		return;
	}

	if (!tile.excavated())
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
		if (!validLanderSite(tile)) { return; }

		auto& s = *new ColonistLander(&tile);
		s.deploySignal().connect({this, &MapViewState::onDeployColonistLander});
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
		if (!validLanderSite(tile)) { return; }

		auto& cargoLander = *new CargoLander(&tile);
		cargoLander.deploySignal().connect({this, &MapViewState::onDeployCargoLander});
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

		auto& structure = *StructureCatalogue::get(mCurrentStructure);
		NAS2D::Utility<StructureManager>::get().addStructure(structure, tile);

		if (structure.isFactory())
		{
			auto& factory = static_cast<Factory&>(structure);
			factory.productionComplete().connect({this, &MapViewState::onFactoryProductionComplete});
			factory.resourcePool(&mResourcesCount);
		}

		if (structure.structureId() == StructureID::SID_MAINTENANCE_FACILITY)
		{
			static_cast<MaintenanceFacility&>(structure).resources(mResourcesCount);
		}

		auto cost = StructureCatalogue::costToBuild(mCurrentStructure);
		removeRefinedResources(cost);
		updatePlayerResources();
		updateStructuresAvailability();
	}
}


void MapViewState::placeRobot(Tile& tile)
{
	if (!tile.excavated()) { return; }
	if (!mRobotPool.isControlCapacityAvailable()) { return; }

	if (!inCommRange(tile.xy()))
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertOutOfCommRange);
		return;
	}

	switch (mCurrentRobot)
	{
	case Robot::Type::Dozer:
		placeRobodozer(tile);
		break;
	case Robot::Type::Digger:
		placeRobodigger(tile);
		break;
	case Robot::Type::Miner:
		placeRobominer(tile);
		break;
	default:
		break;
	}
}


void MapViewState::placeRobodozer(Tile& tile)
{
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
		const auto tilePosition = mDetailMap->mouseTilePosition().xy;
		mTileMap->removeMineLocation(tilePosition);
		tile.pushMine(nullptr);
		for (int i = 0; i <= mTileMap->maxDepth(); ++i)
		{
			auto& mineShaftTile = mTileMap->getTile({tilePosition, i});
			NAS2D::Utility<StructureManager>::get().removeStructure(*mineShaftTile.structure());
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

		if (structure->isLander() && structure->age() == 0)
		{
			doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertCannotBulldozeLandingSite);
			return;
		}

		if (structure->isRobotCommand())
		{
			if (mRobotPool.currentControlCount() >= mRobotPool.robotControlMax() - 10)
			{
				mNotificationArea.push({
					"Cannot bulldoze",
					"Cannot bulldoze Robot Command Center by a Robot under its command.",
					tile.xyz(),
					NotificationArea::NotificationType::Information});
				return;
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
			updateCommRangeOverlay();
		}
		if (structure->isPolice())
		{
			updatePoliceOverlay();
		}

		const auto& recycledResources = StructureCatalogue::recyclingValue(structure->structureId());
		const auto& wastedResources = addRefinedResources(recycledResources);

		if (!wastedResources.isEmpty())
		{
			mNotificationArea.push({
				"Resources wasted",
				"Resources wasted demolishing " + structure->name(),
				tile.xyz(),
				NotificationArea::NotificationType::Warning});
		}

		updatePlayerResources();
		updateStructuresAvailability();

		NAS2D::Utility<StructureManager>::get().removeStructure(*structure);
		tile.deleteMapObject();
		updateConnectedness();
	}

	auto& robot = mRobotPool.getDozer();
	robot.startTask(tile);
	mRobotPool.insertRobotIntoTable(mRobotList, robot, tile);

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
		mNotificationArea.push({
			"Mine destroyed",
			"Digger destroyed a Mine at (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ").",
			tile.xyz(),
			NotificationArea::NotificationType::Information});
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

	if (!tile.thing() && mMapView->currentDepth() > 0) { mDiggerDirection.cardinalOnlyEnabled(); }
	else { mDiggerDirection.downOnlyEnabled(); }

	mDiggerDirection.setParameters(tile);

	// If we're placing on the top level we can only ever go down.
	if (mMapView->currentDepth() == constants::DepthSurface)
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
	if (mMapView->currentDepth() != constants::DepthSurface)
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
	robot.startTask(tile);
	mRobotPool.insertRobotIntoTable(mRobotList, robot, tile);

	if (!mRobotPool.robotAvailable(Robot::Type::Miner))
	{
		mRobots.removeItem(constants::Robominer);
		clearMode();
	}
}


Robot& MapViewState::addRobot(Robot::Type type)
{
	const std::map<Robot::Type, void (MapViewState::*)(Robot*)> RobotTypeToHandler
	{
		{Robot::Type::Digger, &MapViewState::onDiggerTaskComplete},
		{Robot::Type::Dozer, &MapViewState::onDozerTaskComplete},
		{Robot::Type::Miner, &MapViewState::onMinerTaskComplete},
	};

	if (RobotTypeToHandler.find(type) == RobotTypeToHandler.end())
	{
		throw std::runtime_error("Unknown Robot::Type: " + std::to_string(static_cast<int>(type)));
	}

	auto& robot = mRobotPool.addRobot(type);
	robot.taskComplete().connect({this, RobotTypeToHandler.at(type)});
	return robot;
}


/**
 * Checks the robot selection interface and if the robot is not available in it, adds
 * it back in.
 */
void MapViewState::populateRobotMenu()
{
	mRobots.clear();

	for (auto& [robotType, robotMeta] : RobotMetaTable)
	{
		if (mRobotPool.robotAvailable(robotType))
		{
			mRobots.addItem({robotMeta.name, robotMeta.sheetIndex, static_cast<int>(robotType)});
		}
	}

	mRobots.sort();
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

		auto& s = *new SeedLander(point);
		s.deploySignal().connect({this, &MapViewState::onDeploySeedLander});
		NAS2D::Utility<StructureManager>::get().addStructure(s, mTileMap->getTile({point, 0})); // Can only ever be placed on depth level 0

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

		const auto& position = tile->xyz();

		pushAgingRobotMessage(robot, position, mNotificationArea);

		if (robot->isDead())
		{
			const auto robotLocationText = "(" +  std::to_string(position.xy.x) + ", " + std::to_string(position.xy.y) + ")";

			if (robot->selfDestruct())
			{
				mNotificationArea.push({
					"Robot Self-Destructed",
					robot->name() + " at location " + robotLocationText + " self destructed.",
					position,
					NotificationArea::NotificationType::Critical
				});
			}
			else if (robot->type() != Robot::Type::Miner)
			{
				const auto text = "Your " + robot->name() + " at location " + robotLocationText + " has broken down. It will not be able to complete its task and will be removed from your inventory.";
				mNotificationArea.push({"Robot Broke Down", text, position, NotificationArea::NotificationType::Critical});
				robot->abortTask(*tile);
			}

			if (tile->thing() == robot)
			{
				tile->removeMapObject();
			}

			if (mRobotInspector.focusedRobot() == robot) { mRobotInspector.hide(); }

			mRobotPool.erase(robot);
			robot_it = mRobotList.erase(robot_it);
		}
		else if (robot->idle())
		{
			if (tile->thing() == robot)
			{
				tile->removeMapObject();

				mNotificationArea.push({
					"Robot Task Completed",
					robot->name() + " completed its task at" + std::to_string(tile->xy().x) + ", " + std::to_string(tile->xy().y) + ").",
					tile->xyz(),
					NotificationArea::NotificationType::Success
				});
			}
			robot_it = mRobotList.erase(robot_it);

			if (robot->taskCanceled())
			{
				robot->abortTask(*tile);
				populateRobotMenu();
				robot->reset();

				mNotificationArea.push({
					"Robot Task Canceled",
					robot->name() + " canceled its task at" + std::to_string(tile->xy().x) + ", " + std::to_string(tile->xy().y) + ").",
					tile->xyz(),
					NotificationArea::NotificationType::Information
				});
			}
		}
		else
		{
			++robot_it;
		}
	}

	mRobotPool.update();
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
void MapViewState::updateConnectedness()
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	structureManager.updateConnectedness(*mTileMap);
	mConnectednessOverlay = structureManager.getConnectednessOverlay();
}


void MapViewState::updateCommRangeOverlay()
{
	mCommRangeOverlay.clear();

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	fillOverlay(*mTileMap, mCommRangeOverlay, structureManager.getStructures<CommandCenter>());
	fillOverlay(*mTileMap, mCommRangeOverlay, structureManager.getStructures<CommTower>());
}


void MapViewState::updatePoliceOverlay()
{
	resetPoliceOverlays();

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	fillOverlay(*mTileMap, mPoliceOverlays[0], structureManager.getStructures<SurfacePolice>());
	fillOverlay(*mTileMap, mPoliceOverlays, structureManager.getStructures<UndergroundPolice>());
}


void MapViewState::resetPoliceOverlays()
{
	const auto adjustedZ = mTileMap->maxDepth() + 1;
	mPoliceOverlays = std::vector<std::vector<Tile*>>(static_cast<std::size_t>(adjustedZ));
}


/**
 * Removes deployed robots from the TileMap to
 * prevent dangling pointers. Yay for raw memory!
 */
void MapViewState::scrubRobotList()
{
	for (auto it : mRobotList)
	{
		it.second->removeMapObject();
	}
}


bool MapViewState::hasGameEnded()
{
	return mFade.isFaded();
}
