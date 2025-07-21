#include "MapViewState.h"

#include "MapViewStateHelper.h"

#include "MainMenuState.h"
#include "ReportsState.h"
#include "GameState.h"
#include "ColonyShip.h"

#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"

#include "../PointerType.h"
#include "../StructureCatalog.h"
#include "../StructureManager.h"

#include "../Map/Route.h"
#include "../Map/RouteFinder.h"
#include "../Map/Tile.h"
#include "../Map/TileMap.h"
#include "../Map/MapView.h"

#include "../MapObjects/Robots.h"
#include "../MapObjects/RobotTypeIndex.h"

#include "../MapObjects/Structures/CargoLander.h"
#include "../MapObjects/Structures/ColonistLander.h"
#include "../MapObjects/Structures/CommandCenter.h"
#include "../MapObjects/Structures/Factory.h"
#include "../MapObjects/Structures/MaintenanceFacility.h"
#include "../MapObjects/Structures/MineFacility.h"
#include "../MapObjects/Structures/SeedLander.h"
#include "../MapObjects/Structures/StorageTanks.h"
#include "../MapObjects/Structures/Tube.h"
#include "../MapObjects/Structures/Warehouse.h"

#include "../UI/MessageBox.h"
#include "../UI/DetailMap.h"
#include "../UI/NavControl.h"
#include "../UI/MiniMap.h"

#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/DirectionOffset.h>
#include <libOPHD/MeanSolarDistance.h>
#include <libOPHD/ProductCatalog.h>
#include <libOPHD/Population/MoraleChangeEntry.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/StringFrom.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/EnumKeyCode.h>
#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/PointInRectangleRange.h>

#include <algorithm>
#include <sstream>
#include <vector>


extern NAS2D::Point<int> MOUSE_COORDS;


namespace
{
	// Relative proportion of Ore Deposits with yields {low, med, high}
	const std::map<PlanetHostility, std::array<int, 3>> HostilityOreDepositYields =
	{
		{PlanetHostility::Low, {30, 50, 20}},
		{PlanetHostility::Medium, {45, 35, 20}},
		{PlanetHostility::High, {35, 20, 45}},
	};

	struct RobotMeta
	{
		std::string name;
		const int sheetIndex;
	};

	const std::map<RobotTypeIndex, RobotMeta> RobotMetaTable
	{
		{RobotTypeIndex::Digger, RobotMeta{constants::Robodigger, constants::RobodiggerSheetId}},
		{RobotTypeIndex::Dozer, RobotMeta{constants::Robodozer, constants::RobodozerSheetId}},
		{RobotTypeIndex::Miner, RobotMeta{constants::Robominer, constants::RobominerSheetId}}
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


	void fillOverlayCircle(TileMap& tileMap, std::vector<Tile*>& tileList, const Tile& centerTile, int range)
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


	void pushAgingRobotMessage(const Robot* robot, const MapCoordinate& position, NotificationArea& notificationArea)
	{
		if (robot->fuelCellAge() == 190) // FIXME: magic number
		{
			notificationArea.push({
				"Aging Robot",
				"Robot '" + robot->name() + "' at location " + NAS2D::stringFrom(position.xy) + " is approaching its maximum age.",
				position,
				NotificationArea::NotificationType::Warning});
		}
		else if (robot->fuelCellAge() == 195) // FIXME: magic number
		{
			notificationArea.push({
				"Aging Robot",
				"Robot '" + robot->name() + "' at location " + NAS2D::stringFrom(position.xy) + " will fail in a few turns. Replace immediately.",
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


MapViewState::MapViewState(GameState& gameState, NAS2D::Xml::XmlDocument& saveGameDocument, EventDelegate quitHandler) :
	mDifficulty{Difficulty::Medium},
	mCrimeRateUpdate{mDifficulty},
	mCrimeExecution{mDifficulty, {this, &MapViewState::onCrimeEvent}},
	mColonyShip{gameState.colonyShip()},
	mTechnologyReader{"tech0-1.xml"},
	mTurnNumberOfLanding{constants::ColonyShipOrbitTime},
	mLoadingExisting{true},
	mExistingToLoad{&saveGameDocument},
	mReportsState{gameState.reportsState()},
	mInsertMode{InsertMode::None},
	mCurrentStructure{StructureID::SID_NONE},
	mCurrentRobot{RobotTypeIndex::None},
	mStructures{{this, &MapViewState::onStructuresSelectionChange}, "ui/structures.png", constants::StructureIconSize, constants::MarginTight, true},
	mRobots{{this, &MapViewState::onRobotsSelectionChange}, "ui/robots.png", constants::RobotIconSize, constants::MarginTight, true},
	mConnections{{this, &MapViewState::onConnectionsSelectionChange}, "ui/structures.png", constants::StructureIconSize, constants::MarginTight},
	mBtnTurns{Control::getImage("ui/icons/turns.png"), {this, &MapViewState::onTurns}},
	mBtnToggleHeightmap{Control::getImage("ui/icons/height.png"), {this, &MapViewState::onToggleHeightmap}},
	mBtnToggleRouteOverlay{Control::getImage("ui/icons/route.png"), {this, &MapViewState::onToggleRouteOverlay}},
	mBtnToggleConnectedness{Control::getImage("ui/icons/connection.png"), {this, &MapViewState::onToggleConnectedness}},
	mBtnToggleCommRangeOverlay{Control::getImage("ui/icons/comm_overlay.png"), {this, &MapViewState::onToggleCommRangeOverlay}},
	mBtnTogglePoliceOverlay{Control::getImage("ui/icons/police.png"), {this, &MapViewState::onTogglePoliceOverlay}},
	mCheatMenu{{this, &MapViewState::onCheatCodeEntry}},
	mDiggerDirection{{this, &MapViewState::onDiggerSelectionDialog}},
	mFileIoDialog{gameState.fileIoDialog()},
	mGameOverDialog{{this, &MapViewState::onGameOver}},
	mGameOptionsDialog{
		{this, &MapViewState::onOpenSaveGameDialog},
		{this, &MapViewState::onOpenLoadGameDialog},
		{this, &MapViewState::onGameOver},
		{this, &MapViewState::onReturnToGame},
	},
	mNotificationArea{{this, &MapViewState::onNotificationClicked}},
	mNotificationWindow{{this, &MapViewState::onTakeMeThere}},
	mPopulationPanel{mPopulation, mPopulationPool, mMorale},
	mQuitHandler{quitHandler},
	mResourceInfoBar{mResourcesCount, mPopulation, mMorale, mFood},
	mRobotDeploymentSummary{mRobotPool}
{
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect({this, &MapViewState::onWindowResized});
}


MapViewState::MapViewState(GameState& gameState, const PlanetAttributes& planetAttributes, Difficulty selectedDifficulty, EventDelegate quitHandler) :
	mDifficulty{selectedDifficulty},
	mTileMap{std::make_unique<TileMap>(planetAttributes.mapImagePath, planetAttributes.maxDepth, planetAttributes.maxOreDeposits, HostilityOreDepositYields.at(planetAttributes.hostility))},
	mCrimeRateUpdate{mDifficulty},
	mCrimeExecution{mDifficulty, {this, &MapViewState::onCrimeEvent}},
	mColonyShip{gameState.colonyShip()},
	mTechnologyReader{"tech0-1.xml"},
	mPlanetAttributes{planetAttributes},
	mTurnNumberOfLanding{constants::ColonyShipOrbitTime},
	mReportsState{gameState.reportsState()},
	mMapView{std::make_unique<MapView>(*mTileMap)},
	mInsertMode{InsertMode::None},
	mCurrentStructure{StructureID::SID_NONE},
	mCurrentRobot{RobotTypeIndex::None},
	mStructures{{this, &MapViewState::onStructuresSelectionChange}, "ui/structures.png", constants::StructureIconSize, constants::MarginTight, true},
	mRobots{{this, &MapViewState::onRobotsSelectionChange}, "ui/robots.png", constants::RobotIconSize, constants::MarginTight, true},
	mConnections{{this, &MapViewState::onConnectionsSelectionChange}, "ui/structures.png", constants::StructureIconSize, constants::MarginTight},
	mBtnTurns{Control::getImage("ui/icons/turns.png"), {this, &MapViewState::onTurns}},
	mBtnToggleHeightmap{Control::getImage("ui/icons/height.png"), {this, &MapViewState::onToggleHeightmap}},
	mBtnToggleRouteOverlay{Control::getImage("ui/icons/route.png"), {this, &MapViewState::onToggleRouteOverlay}},
	mBtnToggleConnectedness{Control::getImage("ui/icons/connection.png"), {this, &MapViewState::onToggleConnectedness}},
	mBtnToggleCommRangeOverlay{Control::getImage("ui/icons/comm_overlay.png"), {this, &MapViewState::onToggleCommRangeOverlay}},
	mBtnTogglePoliceOverlay{Control::getImage("ui/icons/police.png"), {this, &MapViewState::onTogglePoliceOverlay}},
	mCheatMenu{{this, &MapViewState::onCheatCodeEntry}},
	mDiggerDirection{{this, &MapViewState::onDiggerSelectionDialog}},
	mFileIoDialog{gameState.fileIoDialog()},
	mGameOverDialog{{this, &MapViewState::onGameOver}},
	mGameOptionsDialog{
		{this, &MapViewState::onOpenSaveGameDialog},
		{this, &MapViewState::onOpenLoadGameDialog},
		{this, &MapViewState::onGameOver},
		{this, &MapViewState::onReturnToGame},
	},
	mNotificationArea{{this, &MapViewState::onNotificationClicked}},
	mNotificationWindow{{this, &MapViewState::onTakeMeThere}},
	mPopulationPanel{mPopulation, mPopulationPool, mMorale},
	mQuitHandler{quitHandler},
	mPoliceOverlays{static_cast<std::vector<Tile*>::size_type>(mTileMap->maxDepth() + 1)},
	mResourceInfoBar{mResourcesCount, mPopulation, mMorale, mFood},
	mRobotDeploymentSummary{mRobotPool},
	mMiniMap{std::make_unique<MiniMap>(*mMapView, *mTileMap, mDeployedRobots, planetAttributes.mapImagePath)},
	mDetailMap{std::make_unique<DetailMap>(*mMapView, *mTileMap, planetAttributes.tilesetPath)},
	mNavControl{std::make_unique<NavControl>(*mMapView)}
{
	setMeanSolarDistance(mPlanetAttributes.meanSolarDistance);
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect({this, &MapViewState::onWindowResized});
}


MapViewState::~MapViewState()
{
	scrubRobotList();

	setCursor(PointerType::Normal);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.activate().disconnect({this, &MapViewState::onActivate});
	eventHandler.keyDown().disconnect({this, &MapViewState::onKeyDown});
	eventHandler.mouseButtonDown().disconnect({this, &MapViewState::onMouseDown});
	eventHandler.mouseButtonUp().disconnect({this, &MapViewState::onMouseUp});
	eventHandler.mouseDoubleClick().disconnect({this, &MapViewState::onMouseDoubleClick});
	eventHandler.mouseMotion().disconnect({this, &MapViewState::onMouseMove});
	eventHandler.mouseWheel().disconnect({this, &MapViewState::onMouseWheel});
	eventHandler.windowResized().disconnect({this, &MapViewState::onWindowResized});

	NAS2D::Utility<std::map<const MineFacility*, Route>>::get().clear();
}


/**
 * Initialize values, the UI and set up event handling.
 */
void MapViewState::initialize()
{
	// UI
	initUi();
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	setCursor(PointerType::Normal);

	mPopulationPool.population(&mPopulation);

	StructureCatalog::init("StructureTypes.xml");
	ProductCatalog::init("factory_products.xml");

	if (mLoadingExisting)
	{
		load(mExistingToLoad);
	}

	mResourceInfoBar.ignoreGlow(mTurnCount == 0);

	setupUiPositions(renderer.size());

	mReportsState.injectTechnology(mTechnologyReader, mResearchTracker);

	mFade.fadeIn(constants::FadeSpeed);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();

	eventHandler.activate().connect({this, &MapViewState::onActivate});
	eventHandler.keyDown().connect({this, &MapViewState::onKeyDown});
	eventHandler.mouseButtonDown().connect({this, &MapViewState::onMouseDown});
	eventHandler.mouseButtonUp().connect({this, &MapViewState::onMouseUp});
	eventHandler.mouseDoubleClick().connect({this, &MapViewState::onMouseDoubleClick});
	eventHandler.mouseMotion().connect({this, &MapViewState::onMouseMove});
	eventHandler.mouseWheel().connect({this, &MapViewState::onMouseWheel});

	mPathSolver = std::make_unique<RouteFinder>(*mTileMap);
}


void MapViewState::onActivate()
{
	unhideUi();
}


void MapViewState::onDeactivate()
{
	mGameOverDialog.enabled(false);
	mGameOptionsDialog.enabled(false);

	hideUi();
}


void MapViewState::focusOnStructure(const Structure* structure)
{
	if (!structure) { return; }
	onTakeMeThere(structure->xyz());
}


void MapViewState::difficulty(Difficulty difficulty)
{
	mDifficulty = difficulty;
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
	mDetailMap->draw(renderer);

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
	const auto& storageTanks = NAS2D::Utility<StructureManager>::get().getStructures<StorageTanks>();
	const auto& command = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();

	std::vector<Structure*> storage;
	storage.insert(storage.end(), command.begin(), command.end());
	storage.insert(storage.end(), storageTanks.begin(), storageTanks.end());

	StorableResources resources;
	for (auto* structure : storage)
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
void MapViewState::onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool /*repeat*/)
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

	if (key == NAS2D::KeyCode::F1)
	{
		mReportsState.showReport();
		return;
	}

	const int shiftMoveScalar = NAS2D::EventHandler::shift(mod) ? 5 : 1;

	switch(key)
	{

		case NAS2D::KeyCode::W:
		case NAS2D::KeyCode::Up:
			moveView(MapOffsetNorthWest * shiftMoveScalar);
			break;

		case NAS2D::KeyCode::S:
		case NAS2D::KeyCode::Down:
			moveView(MapOffsetSouthEast * shiftMoveScalar);
			break;

		case NAS2D::KeyCode::A:
		case NAS2D::KeyCode::Left:
			moveView(MapOffsetSouthWest * shiftMoveScalar);
			break;

		case NAS2D::KeyCode::D:
		case NAS2D::KeyCode::Right:
			moveView(MapOffsetNorthEast * shiftMoveScalar);
			break;

		case NAS2D::KeyCode::Num0:
			changeViewDepth(0);
			break;

		case NAS2D::KeyCode::Num1:
			changeViewDepth(1);
			break;

		case NAS2D::KeyCode::Num2:
			changeViewDepth(2);
			break;

		case NAS2D::KeyCode::Num3:
			changeViewDepth(3);
			break;

		case NAS2D::KeyCode::Num4:
			changeViewDepth(4);
			break;

		case NAS2D::KeyCode::PageUp:
			moveView(MapOffsetUp);
			break;

		case NAS2D::KeyCode::PageDown:
			moveView(MapOffsetDown);
			break;


		case NAS2D::KeyCode::Home:
			changeViewDepth(0);
			break;

		case NAS2D::KeyCode::End:
			changeViewDepth(mTileMap->maxDepth());
			break;

		case NAS2D::KeyCode::F10:
			if (NAS2D::Utility<NAS2D::EventHandler>::get().control(mod) && NAS2D::Utility<NAS2D::EventHandler>::get().shift(mod))
			{
				mCheatMenu.show();
				mWindowStack.bringToFront(mCheatMenu);
			}
			break;

		case NAS2D::KeyCode::F2:
			mFileIoDialog.showSave(constants::SaveGamePath);
			break;

		case NAS2D::KeyCode::F3:
			mFileIoDialog.showOpen(constants::SaveGamePath);
			break;

		case NAS2D::KeyCode::Escape:
			resetUi();
			break;

		case NAS2D::KeyCode::Enter:
			if (mBtnTurns.enabled()) { nextTurn(); }
			break;

		default:
			break;
	}
}


void MapViewState::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!active()) { return; }

	if (modalUiElementDisplayed()) { return; }

	if (mWindowStack.pointInWindow(MOUSE_COORDS))
	{
		mWindowStack.updateStack(MOUSE_COORDS);
		return;
	}

	if (button == NAS2D::MouseButton::Right || button == NAS2D::MouseButton::Middle)
	{
		if (isInserting())
		{
			resetUi();
			return;
		}

		if (!mDetailMap->isMouseOverTile()) { return; }
		const auto tilePosition = mDetailMap->mouseTilePosition();
		if (!mTileMap->isValidPosition(tilePosition)) { return; }

		const bool inspectModifier = NAS2D::Utility<NAS2D::EventHandler>::get().shift() ||
			button == NAS2D::MouseButton::Middle;

		onInspect(tilePosition, inspectModifier);
	}

	if (button == NAS2D::MouseButton::Left)
	{
		if (mTooltipSystemButton.area().contains(MOUSE_COORDS))
		{
			onSystemMenu();
		}

		const auto oldDepth = mMapView->currentDepth();
		mNavControl->onClick(MOUSE_COORDS);
		if (oldDepth != mMapView->currentDepth())
		{
			onChangeDepth(oldDepth, mMapView->currentDepth());
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


void MapViewState::onMouseDoubleClick(NAS2D::MouseButton button, NAS2D::Point<int> /*position*/)
{
	if (!active()) { return; }

	if (button == NAS2D::MouseButton::Left)
	{
		if (mWindowStack.pointInWindow(MOUSE_COORDS)) { return; }
		if (!mDetailMap->isMouseOverTile()) { return; }
		const auto tilePosition = mDetailMap->mouseTilePosition();
		if (!mTileMap->isValidPosition(tilePosition)) { return; }

		auto& tile = mTileMap->getTile(tilePosition);
		if (tile.hasStructure())
		{
			mReportsState.showReport(*tile.structure());
		}
	}
}


void MapViewState::onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (button == NAS2D::MouseButton::Left)
	{
		mMiniMap->onMouseUp(button, position);
	}
}


void MapViewState::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative)
{
	if (!active()) { return; }
	mMiniMap->onMouseMove(position, relative);
}


void MapViewState::onMapObjectSelectionChanged()
{
	setCursor(isInserting() ? PointerType::PlaceTile : PointerType::Normal);
}


void MapViewState::onInspect(const MapCoordinate& tilePosition, bool inspectModifier)
{
	auto& tile = mTileMap->getTile(tilePosition);
	if (tile.empty())
	{
		onInspectTile(tile);
	}
	else if (tile.hasRobot())
	{
		onInspectRobot(*tile.robot());
	}
	else if (tile.hasStructure())
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
		mFactoryProduction.factory(&dynamic_cast<Factory&>(structure));
		mFactoryProduction.show();
		mWindowStack.bringToFront(mFactoryProduction);
	}
	else if (structure.isWarehouse() && preferStructureSpecificView)
	{
		mWarehouseInspector.warehouse(&dynamic_cast<Warehouse&>(structure));
		mWarehouseInspector.show();
		mWindowStack.bringToFront(mWarehouseInspector);
	}
	else if (structure.isMineFacility() && preferStructureSpecificView)
	{
		mMineOperationsWindow.mineFacility(&dynamic_cast<MineFacility&>(structure));
		mMineOperationsWindow.show();
		mWindowStack.bringToFront(mMineOperationsWindow);
	}
	else
	{
		mStructureInspector.showStructure(structure);
		mWindowStack.bringToFront(mStructureInspector);
	}
}


void MapViewState::onInspectRobot(Robot& robot)
{
	mRobotInspector.focusOnRobot(&robot);
	mRobotInspector.show();
	mWindowStack.bringToFront(mRobotInspector);
}


void MapViewState::onInspectTile(Tile& tile)
{
	mTileInspector.tile(tile);
	mTileInspector.show();
	mWindowStack.bringToFront(mTileInspector);
}


void MapViewState::onClickMap()
{
	if (!mDetailMap->isMouseOverTile()) { return; }
	Tile& tile = mDetailMap->mouseTile();

	if (mInsertMode == InsertMode::Structure)
	{
		placeStructure(tile, mCurrentStructure);
	}
	else if (mInsertMode == InsertMode::Robot)
	{
		placeRobot(tile, mCurrentRobot);
	}
	else if (mInsertMode == InsertMode::Tube)
	{
		/** FIXME: This is a kludge that only works because all of the tube structures are listed alphabetically.
		* Should instead take advantage of the updated meta data in the IconGridItem.
		*/
		auto connectorDirection = static_cast<ConnectorDir>(mConnections.selectedIndex() + 1);

		placeTubes(tile, connectorDirection);
	}
}


void MapViewState::onSystemMenu()
{
	mGameOptionsDialog.show();
	resetUi();
}

/**
* Handle side effects of changing depth view
*/
void MapViewState::onChangeDepth(int oldDepth, int newDepth) {
	if (mBtnTogglePoliceOverlay.isPressed())
	{
		changePoliceOverlayDepth(oldDepth, newDepth);
	}

	if (!isInsertingRobot()) { clearBuildMode(); }

	populateStructureMenu();
}

void MapViewState::moveView(MapOffset offset) {
	int oldZLevel = mMapView->currentDepth();
	mMapView->moveView(offset);

	int newZLevel = mMapView->currentDepth();
	if (oldZLevel != newZLevel) {
		onChangeDepth(oldZLevel, newZLevel);
	}
}

/**
 * Changes the current view depth.
 */
void MapViewState::changeViewDepth(int depth)
{
	int oldZLevel = mMapView->currentDepth();

	mMapView->currentDepth(depth);

	int newZLevel = mMapView->currentDepth();
	if (oldZLevel != newZLevel) {
		onChangeDepth(oldZLevel, newZLevel);
	}
}


void MapViewState::insertTube(Tile& tile, ConnectorDir dir)
{
	if (dir == ConnectorDir::CONNECTOR_VERTICAL)
	{
		throw std::runtime_error("MapViewState::insertTube() called with invalid ConnectorDir parameter.");
	}

	NAS2D::Utility<StructureManager>::get().addStructure(*new Tube(tile, dir), tile);
}


void MapViewState::placeTubes(Tile& tile, ConnectorDir connectorDirection)
{
	if (!tile.isBulldozed()) {
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertTubeTerrain);
		return;
	}

	if (tile.mapObject() || tile.oreDeposit() || !tile.excavated()) { return; }

	if (validTubeConnection(*mTileMap, tile.xyz(), connectorDirection))
	{
		insertTube(tile, connectorDirection);

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
void MapViewState::placeStructure(Tile& tile, StructureID structureID)
{
	if (structureID == StructureID::SID_NONE) { throw std::runtime_error("MapViewState::placeStructure() called but structureID == STRUCTURE_NONE"); }

	if (!selfSustained(structureID) && !isInCcRange(tile.xy()))
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureOutOfRange);
		return;
	}

	if (mTileMap->isTileBlockedByOreDeposit(tile))
	{
		doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureOreDepositInWay);
		return;
	}

	if (tile.mapObject())
	{
		if (tile.hasStructure())
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTileObstructed);
		}
		else
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureTileMapObject);
		}
		return;
	}

	if ((!tile.isBulldozed() && !structureIsLander(structureID)))
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
	if (structureID == StructureID::SID_SEED_LANDER)
	{
		insertSeedLander(tile.xy());
	}
	else if (structureID == StructureID::SID_COLONIST_LANDER)
	{
		if (!validLanderSite(tile)) { return; }

		auto& s = NAS2D::Utility<StructureManager>::get().create<ColonistLander>(tile);
		s.deployHandler({this, &MapViewState::onDeployColonistLander});

		mColonyShip.onDeployColonistLander();
		if (mColonyShip.colonistLanders() == 0)
		{
			resetUi();
			populateStructureMenu();
		}
	}
	else if (structureID == StructureID::SID_CARGO_LANDER)
	{
		if (!validLanderSite(tile)) { return; }

		auto& cargoLander = NAS2D::Utility<StructureManager>::get().create<CargoLander>(tile);
		cargoLander.deployHandler({this, &MapViewState::onDeployCargoLander});

		mColonyShip.onDeployCargoLander();
		if (mColonyShip.cargoLanders() == 0)
		{
			resetUi();
			populateStructureMenu();
		}
	}
	else
	{
		if (!validStructurePlacement(*mTileMap, tile.xyz()) && !selfSustained(structureID))
		{
			doAlertMessage(constants::AlertInvalidStructureAction, constants::AlertStructureNoTube);
			return;
		}

		// Check build cost
		if (!StructureCatalog::canBuild(structureID, mResourcesCount))
		{
			resourceShortageMessage(mResourcesCount, structureID);
			return;
		}

		auto& structure = NAS2D::Utility<StructureManager>::get().create(structureID, tile);

		if (structure.isFactory())
		{
			auto& factory = dynamic_cast<Factory&>(structure);
			factory.productionCompleteHandler({this, &MapViewState::onFactoryProductionComplete});
			factory.resourcePool(&mResourcesCount);
		}

		if (structure.isMaintenance())
		{
			dynamic_cast<MaintenanceFacility&>(structure).resources(mResourcesCount);
		}

		auto cost = StructureCatalog::costToBuild(structureID);
		removeRefinedResources(cost);
		updatePlayerResources();
		updateStructuresAvailability();
	}
}


void MapViewState::placeRobot(Tile& tile, RobotTypeIndex robotTypeIndex)
{
	if (!tile.excavated()) { return; }
	if (!mRobotPool.isControlCapacityAvailable()) { return; }

	if (!isInCommRange(tile.xy()))
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertOutOfCommRange);
		return;
	}

	switch (robotTypeIndex)
	{
	case RobotTypeIndex::Dozer:
		placeRobodozer(tile);
		break;
	case RobotTypeIndex::Digger:
		placeRobodigger(tile);
		break;
	case RobotTypeIndex::Miner:
		placeRobominer(tile);
		break;
	default:
		break;
	}
}


void MapViewState::placeRobodozer(Tile& tile)
{
	if (tile.mapObject() && !tile.hasStructure())
	{
		return;
	}
	else if (tile.isBulldozed() && !tile.hasStructure())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertTileBulldozed);
		return;
	}
	else if (tile.oreDeposit())
	{
		if (tile.oreDeposit()->digDepth() != mTileMap->maxDepth() || !tile.oreDeposit()->isExhausted())
		{
			doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMineNotExhausted);
			return;
		}

		mMineOperationsWindow.hide();
		const auto tilePosition = tile.xy();
		mTileMap->removeOreDepositLocation(tilePosition);
		tile.placeOreDeposit(nullptr);
		for (int i = 0; i <= mTileMap->maxDepth(); ++i)
		{
			auto& mineShaftTile = mTileMap->getTile({tilePosition, i});
			NAS2D::Utility<StructureManager>::get().removeStructure(*mineShaftTile.structure());
		}
	}
	else if (tile.hasStructure())
	{
		mStructureInspector.hideStructure(*tile.structure());

		Structure* structure = tile.structure();

		if (structure->isMineFacility()) { return; }
		if (structure->structureClass() == StructureClass::Command)
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

		if (structure->isFactory() && dynamic_cast<Factory*>(structure) == mFactoryProduction.factory())
		{
			mFactoryProduction.hide();
		}

		if (structure->isWarehouse())
		{
			auto* warehouse = dynamic_cast<Warehouse*>(structure);
			if (simulateMoveProducts(warehouse)) { moveProducts(warehouse); }
			else { return; }
		}

		if (structure->structureClass() == StructureClass::Communication)
		{
			updateCommRangeOverlay();
		}
		if (structure->isPolice())
		{
			updatePoliceOverlay();
		}

		const auto& recycledResources = StructureCatalog::recyclingValue(structure->structureId());
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
	mRobotPool.insertRobotIntoTable(mDeployedRobots, robot, tile);

	if (!mRobotPool.robotAvailable(RobotTypeIndex::Dozer))
	{
		mRobots.removeItem(constants::Robodozer);
		clearBuildMode();
	}
}


void MapViewState::placeRobodigger(Tile& tile)
{
	// Keep digger within a safe margin of the map boundaries.
	if (!NAS2D::Rectangle<int>::Create({4, 4}, NAS2D::Point{-4, -4} + mTileMap->size()).contains(tile.xy()))
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertDiggerEdgeBuffer);
		return;
	}

	// Check for obstructions underneath the digger location.
	if (tile.depth() != mTileMap->maxDepth() && !mTileMap->getTile({tile.xy(), tile.depth() + 1}).empty())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertDiggerBlockedBelow);
		return;
	}

	if (tile.hasOreDeposit())
	{
		if (!doYesNoMessage(constants::AlertDiggerMineTile, constants::AlertDiggerMine)) { return; }

		const auto position = tile.xy();
		mNotificationArea.push({
			"Mine destroyed",
			"Digger destroyed a Mine at " + NAS2D::stringFrom(position) + ".",
			tile.xyz(),
			NotificationArea::NotificationType::Information});
		mTileMap->removeOreDepositLocation(position);
	}

	// Die if tile is occupied or not excavated.
	if (!tile.empty())
	{
		if (!tile.isSurface())
		{
			if (tile.hasStructure() && tile.structure()->connectorDirection() != ConnectorDir::CONNECTOR_VERTICAL) // Air shaft
			{
				doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertStructureInWay);
				return;
			}
			else if (tile.hasStructure() && tile.structure()->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL && tile.depth() == mTileMap->maxDepth())
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

	if (!tile.mapObject() && mMapView->currentDepth() > 0) { mDiggerDirection.cardinalOnlyEnabled(); }
	else { mDiggerDirection.downOnlyEnabled(); }

	mDiggerDirection.setParameters(tile);

	// If we're placing on the top level we can only ever go down.
	if (mMapView->isSurface())
	{
		mDiggerDirection.selectDown();
	}
	else
	{
		mDiggerDirection.show();
		mWindowStack.bringToFront(mDiggerDirection);

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
	if (tile.mapObject())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMinerTileObstructed);
		return;
	}
	if (!mMapView->isSurface())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMinerSurfaceOnly);
		return;
	}
	if (!tile.oreDeposit())
	{
		doAlertMessage(constants::AlertInvalidRobotPlacement, constants::AlertMinerNotOnMine);
		return;
	}

	auto& robot = mRobotPool.getMiner();
	robot.startTask(tile);
	mRobotPool.insertRobotIntoTable(mDeployedRobots, robot, tile);

	if (!mRobotPool.robotAvailable(RobotTypeIndex::Miner))
	{
		mRobots.removeItem(constants::Robominer);
		clearBuildMode();
	}
}


Robot& MapViewState::addRobot(RobotTypeIndex robotTypeIndex)
{
	const std::map<RobotTypeIndex, void (MapViewState::*)(Robot&)> RobotTypeIndexToHandler
	{
		{RobotTypeIndex::Digger, &MapViewState::onDiggerTaskComplete},
		{RobotTypeIndex::Dozer, &MapViewState::onDozerTaskComplete},
		{RobotTypeIndex::Miner, &MapViewState::onMinerTaskComplete},
	};

	if (RobotTypeIndexToHandler.find(robotTypeIndex) == RobotTypeIndexToHandler.end())
	{
		throw std::runtime_error("Unknown RobotTypeIndex: " + std::to_string(static_cast<int>(robotTypeIndex)));
	}

	auto& robot = mRobotPool.addRobot(robotTypeIndex);
	robot.taskCompleteHandler({this, RobotTypeIndexToHandler.at(robotTypeIndex)});
	return robot;
}


/**
 * Checks the robot selection interface and if the robot is not available in it, adds
 * it back in.
 */
void MapViewState::populateRobotMenu()
{
	mRobots.clear();

	for (auto& [robotTypeIndex, robotMeta] : RobotMetaTable)
	{
		if (mRobotPool.robotAvailable(robotTypeIndex))
		{
			mRobots.addItem({robotMeta.name, robotMeta.sheetIndex, static_cast<int>(robotTypeIndex)});
		}
	}

	mRobots.sort();
}


/**
 * Checks that the clicked tile is a suitable spot for the SEED Lander and
 * then inserts it into the TileMap.
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

		auto& tile = mTileMap->getTile({point, 0}); // Can only ever be placed on depth level 0
		auto& s = NAS2D::Utility<StructureManager>::get().create<SeedLander>(tile);
		s.deployHandler({this, &MapViewState::onDeploySeedLander});

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
	auto robot_it = mDeployedRobots.begin();
	while (robot_it != mDeployedRobots.end())
	{
		auto& robot = **robot_it;
		auto& tile = robot.tile();

		robot.processTurn(*mTileMap);

		const auto& position = tile.xyz();

		pushAgingRobotMessage(&robot, position, mNotificationArea);

		if (robot.isDead())
		{
			if (robot.selfDestruct())
			{
				mNotificationArea.push({
					"Robot Self-Destructed",
					robot.name() + " at location " + NAS2D::stringFrom(position.xy) + " self destructed.",
					position,
					NotificationArea::NotificationType::Critical
				});
			}
			else if (robot.type() != RobotTypeIndex::Miner)
			{
				const auto text = "Your " + robot.name() + " at location " + NAS2D::stringFrom(position.xy) + " has broken down. It will not be able to complete its task and will be removed from your inventory.";
				mNotificationArea.push({"Robot Broke Down", text, position, NotificationArea::NotificationType::Critical});
				robot.abortTask(tile);
			}

			if (tile.mapObject() == &robot)
			{
				tile.removeMapObject();
			}

			if (mRobotInspector.focusedRobot() == &robot) { mRobotInspector.hide(); }

			mRobotPool.erase(&robot);
			robot_it = mDeployedRobots.erase(robot_it);
		}
		else if (robot.idle())
		{
			if (tile.mapObject() == &robot)
			{
				tile.removeMapObject();

				mNotificationArea.push({
					"Robot Task Completed",
					robot.name() + " completed its task at " + NAS2D::stringFrom(tile.xy()) + ".",
					tile.xyz(),
					NotificationArea::NotificationType::Success
				});
			}
			robot_it = mDeployedRobots.erase(robot_it);

			if (robot.taskCanceled())
			{
				robot.abortTask(tile);
				populateRobotMenu();
				robot.reset();

				mNotificationArea.push({
					"Robot Task Canceled",
					robot.name() + " canceled its task at " + NAS2D::stringFrom(tile.xy()) + ".",
					tile.xyz(),
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
	for (const auto* structure : structureManager.allStructures())
	{
		const auto commRange = structure->commRange();
		if (commRange > 0)
		{
			const auto& centerTile = structure->tile();
			fillOverlayCircle(*mTileMap, mCommRangeOverlay, centerTile, commRange);
		}
	}
}


void MapViewState::updatePoliceOverlay()
{
	for (auto& policeOverlayLevel : mPoliceOverlays)
	{
		policeOverlayLevel.clear();
	}

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	for (const auto* structure : structureManager.allStructures())
	{
		const auto policeRange = structure->policeRange();
		if (policeRange > 0)
		{
			const auto& centerTile = structure->tile();
			const auto depth = static_cast<std::size_t>(centerTile.depth());
			fillOverlayCircle(*mTileMap, mPoliceOverlays[depth], centerTile, policeRange);
		}
	}
}


/**
 * Removes deployed robots from the TileMap to
 * prevent dangling pointers. Yay for raw memory!
 */
void MapViewState::scrubRobotList()
{
	for (auto* robot : mDeployedRobots)
	{
		robot->tile().removeMapObject();
	}
}


bool MapViewState::hasGameEnded()
{
	return mFade.isFaded();
}
