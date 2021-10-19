// ==================================================================================
// = This file implements the UI and UI Event Handlers used by MapViewState. I separated
// = it into its own file because the MapViewState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "MapViewState.h"
#include "MapViewStateHelper.h"

#include "MainMenuState.h"

#include "../Constants.h"
#include "../DirectionOffset.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"
#include "../Map/MapCoordinate.h"
#include "../Map/TileMap.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <cmath>


using namespace NAS2D;


/**
 * Sets up the user interface elements
 * 
 * \note	The explicit casts to int to truncate floating point values to force
 *			window positions to a whole number.
 */
void MapViewState::initUi()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	mDiggerDirection.directionSelected().connect(this, &MapViewState::onDiggerSelectionDialog);
	mDiggerDirection.hide();

	mTileInspector.position(renderer.center() - NAS2D::Vector{mTileInspector.size().x / 2.0f, 175.0f});
	mTileInspector.hide();

	mStructureInspector.position(renderer.center() - NAS2D::Vector{mStructureInspector.size().x / 2.0f, 175.0f});
	mStructureInspector.hide();

	mRobotInspector.position(renderer.center() - NAS2D::Vector{mRobotInspector.size().x / 2.0f, 175.0f});
	mRobotInspector.hide();

	mFactoryProduction.position(NAS2D::Point{renderer.center().x - mFactoryProduction.size().x / 2.0f, 175.0f});
	mFactoryProduction.hide();

	mFileIoDialog.setMode(FileIo::FileOperation::Save);
	mFileIoDialog.fileOperation().connect(this, &MapViewState::onFileIoAction);
	mFileIoDialog.anchored(true);
	mFileIoDialog.hide();

	mPopulationPanel.position({675, constants::ResourceIconSize + 4 + constants::MarginTight});
	mPopulationPanel.population(&mPopulation);

	mResourceBreakdownPanel.position({0, 22});
	mResourceBreakdownPanel.playerResources(&mResourcesCount);

	mGameOverDialog.returnToMainMenu().connect(this, &MapViewState::onGameOver);
	mGameOverDialog.hide();

	mGameOptionsDialog.SaveGame().connect(this, &MapViewState::onSaveGame);
	mGameOptionsDialog.LoadGame().connect(this, &MapViewState::onLoadGame);
	mGameOptionsDialog.returnToGame().connect(this, &MapViewState::onReturnToGame);
	mGameOptionsDialog.returnToMainMenu().connect(this, &MapViewState::onGameOver);
	mGameOptionsDialog.hide();

	mAnnouncement.hide();
	mMineOperationsWindow.hide();
	mWarehouseInspector.hide();

	mWindowStack.addWindow(&mTileInspector);
	mWindowStack.addWindow(&mStructureInspector);
	mWindowStack.addWindow(&mFactoryProduction);
	mWindowStack.addWindow(&mDiggerDirection);
	mWindowStack.addWindow(&mAnnouncement);
	mWindowStack.addWindow(&mWarehouseInspector);
	mWindowStack.addWindow(&mMineOperationsWindow);
	mWindowStack.addWindow(&mRobotInspector);
	mWindowStack.addWindow(&mNotificationWindow);

	mNotificationArea.notificationClicked().connect(this, &MapViewState::onNotificationClicked);

	mNotificationWindow.takeMeThere().connect(this, &MapViewState::onNotificationWindowTakeMeThere);
	mNotificationWindow.hide();

	const auto size = renderer.size().to<int>();
	mBottomUiRect = {0, size.y - constants::BottomUiHeight, size.x, constants::BottomUiHeight};

	// BUTTONS
	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.position(NAS2D::Point{mMiniMapBoundingBox.x - constants::MainButtonSize - constants::MarginTight, size.y - constants::Margin - constants::MainButtonSize});
	mBtnTurns.size(constants::MainButtonSize);
	mBtnTurns.click().connect(this, &MapViewState::onTurns);
	mBtnTurns.enabled(false);

	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(constants::MainButtonSize);
	mBtnToggleHeightmap.type(Button::Type::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(constants::MainButtonSize);
	mBtnToggleConnectedness.type(Button::Type::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().connect(this, &MapViewState::onToggleConnectedness);

	mBtnToggleCommRangeOverlay.image("ui/icons/comm_overlay.png");
	mBtnToggleCommRangeOverlay.size(constants::MainButtonSize);
	mBtnToggleCommRangeOverlay.type(Button::Type::BUTTON_TOGGLE);
	mBtnToggleCommRangeOverlay.click().connect(this, &MapViewState::onToggleCommRangeOverlay);

	mBtnToggleRouteOverlay.image("ui/icons/route.png");
	mBtnToggleRouteOverlay.size(constants::MainButtonSize);
	mBtnToggleRouteOverlay.type(Button::Type::BUTTON_TOGGLE);
	mBtnToggleRouteOverlay.click().connect(this, &MapViewState::onToggleRouteOverlay);

	mBtnTogglePoliceOverlay.image("ui/icons/police.png");
	mBtnTogglePoliceOverlay.size(constants::MainButtonSize);
	mBtnTogglePoliceOverlay.type(Button::Type::BUTTON_TOGGLE);
	mBtnTogglePoliceOverlay.click().connect(this, &MapViewState::onTogglePoliceOverlay);

	// Menus
	mRobots.position({mBtnTurns.positionX() - constants::MarginTight - 52, mBottomUiRect.y + constants::Margin});
	mRobots.size({52, constants::BottomUiHeight - constants::Margin * 2});
	mRobots.showTooltip(true);
	mRobots.selectionChanged().connect(this, &MapViewState::onRobotsSelectionChange);

	mConnections.position({mRobots.positionX() - constants::MarginTight - 52, mBottomUiRect.y + constants::Margin});
	mConnections.size({52, constants::BottomUiHeight - constants::Margin * 2});
	mConnections.selectionChanged().connect(this, &MapViewState::onConnectionsSelectionChange);

	mStructures.position(NAS2D::Point{constants::Margin, mBottomUiRect.y + constants::Margin});
	mStructures.size({mConnections.positionX() - constants::Margin - constants::MarginTight, constants::BottomUiHeight - constants::Margin * 2});
	mStructures.showTooltip(true);
	mStructures.selectionChanged().connect(this, &MapViewState::onStructuresSelectionChange);

	// Initial Structures
	mStructures.addItem(constants::SeedLander, 0, StructureID::SID_SEED_LANDER);

	// tooltip control sizes
	constexpr auto hudHeight = constants::ResourceIconSize + constants::MarginTight * 2;
	mTooltipResourceBreakdown.size({265, hudHeight});

	mTooltipResourceStorage.position({275, 0});
	mTooltipResourceStorage.size({85, hudHeight});

	mTooltipFoodStorage.position({420, 0});
	mTooltipFoodStorage.size({75, hudHeight});

	mTooltipEnergy.position({560, 0});
	mTooltipEnergy.size({55, hudHeight});

	mTooltipPopulation.position({670, 0});
	mTooltipPopulation.size({75, hudHeight});

	mTooltipCurrentTurns.size({45, hudHeight});
	mTooltipSystemButton.size({hudHeight, hudHeight});

	// Tool Tips
	mToolTip.add(mBtnTurns, constants::ToolTipBtnTurns);
	mToolTip.add(mBtnToggleHeightmap, constants::ToolTipBtnHeightmap);
	mToolTip.add(mBtnToggleConnectedness, constants::ToolTipBtnConnectedness);
	mToolTip.add(mBtnToggleCommRangeOverlay, constants::ToolTipBtnCommRange);
	mToolTip.add(mBtnToggleRouteOverlay, constants::ToolTipBtnRoutes);
	mToolTip.add(mBtnTogglePoliceOverlay, constants::ToolTipBtnPolice);
	mToolTip.add(mTooltipResourceBreakdown, constants::ToolTipRefinedResources);
	mToolTip.add(mTooltipResourceStorage, constants::ToolTipResourceStorage);
	mToolTip.add(mTooltipFoodStorage, constants::ToolTipFoodStorage);
	mToolTip.add(mTooltipEnergy, constants::ToolTipEnergy);
	mToolTip.add(mTooltipPopulation, constants::ToolTipPopulation);
	mToolTip.add(mTooltipCurrentTurns, constants::ToolTipCurrentTurns);
	mToolTip.add(mTooltipSystemButton, constants::ToolTipSystemMenu);
}


void MapViewState::setupUiPositions(NAS2D::Vector<int> size)
{
	// Bottom UI Area
	mBottomUiRect = {0, size.y - constants::BottomUiHeight, size.x, constants::BottomUiHeight};

	// Menu / System Icon
	mTooltipSystemButton.position({size.x - (constants::ResourceIconSize + constants::MarginTight * 2), 0});
	mTooltipCurrentTurns.position({size.x - 80 , 0});

	// NAVIGATION BUTTONS
	// Bottom line
	const auto navIconSpacing = 32 + constants::MarginTight;
	mMoveDownIconRect = {size.x - navIconSpacing, size.y - constants::BottomUiHeight - 65, 32, 32};
	mMoveEastIconRect = {mMoveDownIconRect.x - navIconSpacing, mMoveDownIconRect.y + 8, 32, 16};
	mMoveSouthIconRect = {mMoveDownIconRect.x - 2 * navIconSpacing, mMoveDownIconRect.y + 8, 32, 16};

	// Top line
	mMoveUpIconRect = {mMoveDownIconRect.x, mMoveDownIconRect.y - navIconSpacing, 32, 32};
	mMoveNorthIconRect = {mMoveUpIconRect.x - navIconSpacing, mMoveUpIconRect.y + 8, 32, 16};
	mMoveWestIconRect = {mMoveUpIconRect.x - 2 * navIconSpacing, mMoveUpIconRect.y + 8, 32, 16};

	// Notification Area
	auto& renderer = Utility<Renderer>::get();
	mNotificationArea.height(mMoveUpIconRect.y - 22 - constants::Margin);
	mNotificationArea.position({renderer.size().x - mNotificationArea.size().x, 22});

	// Mini Map
	mMiniMapBoundingBox = {size.x - 300 - constants::Margin, mBottomUiRect.y + constants::Margin, 300, 150};

	// Position UI Buttons
	mBtnTurns.position(NAS2D::Point{mMiniMapBoundingBox.x - constants::MainButtonSize - constants::MarginTight, size.y - constants::Margin - constants::MainButtonSize});
	mBtnToggleHeightmap.position({mBtnTurns.positionX(), mMiniMapBoundingBox.y});
	mBtnToggleConnectedness.position({mBtnTurns.positionX(), mMiniMapBoundingBox.y + constants::MainButtonSize});
	mBtnToggleCommRangeOverlay.position({mBtnTurns.positionX(), mMiniMapBoundingBox.y + (constants::MainButtonSize * 2)});
	mBtnToggleRouteOverlay.position({mBtnTurns.positionX(), mMiniMapBoundingBox.y + (constants::MainButtonSize * 3)});
	mBtnTogglePoliceOverlay.position({mBtnTurns.positionX() - constants::MainButtonSize, mMiniMapBoundingBox.y});

	// UI Panels
	mRobots.position({mBtnTogglePoliceOverlay.positionX() - constants::MarginTight - 52, mBottomUiRect.y + constants::Margin});
	mConnections.position({mRobots.positionX() - constants::MarginTight - 52, mBottomUiRect.y + constants::Margin});
	mStructures.position(NAS2D::Point{constants::Margin, mBottomUiRect.y + constants::Margin});

	mStructures.size({mConnections.positionX() - constants::Margin - constants::MarginTight, constants::BottomUiHeight - constants::Margin * 2});

	// Allow for centering with rounding to integer values
	const auto rendererCenter = NAS2D::Utility<NAS2D::Renderer>::get().center().to<int>();
	const auto centerPosition = [&rendererCenter](const Control& control) { return (rendererCenter - control.size() / 2); };

	// Anchored window positions
	mFileIoDialog.position(NAS2D::Point{centerPosition(mFileIoDialog).x, 50});
	mGameOverDialog.position(centerPosition(mGameOverDialog) - NAS2D::Vector{0, 100});
	mAnnouncement.position(centerPosition(mAnnouncement) - NAS2D::Vector{0, 100});
	mGameOptionsDialog.position(centerPosition(mGameOptionsDialog) - NAS2D::Vector{0, 100});

	mDiggerDirection.position(NAS2D::Point{centerPosition(mDiggerDirection).x, size.y / 2 - 125});

	mWarehouseInspector.position(centerPosition(mWarehouseInspector) - NAS2D::Vector{0, 100});
	mMineOperationsWindow.position(centerPosition(mMineOperationsWindow) - NAS2D::Vector{0, 100});

	mNotificationWindow.position(centerPosition(mMineOperationsWindow) - NAS2D::Vector{0, 100});

	/**
	 * \note	We are not setting the tile inspector window's position here because it's something that can be
	 *			move arround by the user. Re-centering it here could annoy the user.
	 */
}


/**
 * Hides ALL UI elements.
 */
void MapViewState::hideUi()
{
	mBtnTurns.hide();

	mBtnToggleHeightmap.hide();
	mBtnToggleConnectedness.hide();
	mBtnToggleCommRangeOverlay.hide();
	mBtnToggleRouteOverlay.hide();
	mBtnTogglePoliceOverlay.hide();

	mStructures.hide();
	mRobots.hide();
	mConnections.hide();

	mWindowStack.hide();
}


/**
 * Unhides the UI.
 */
void MapViewState::unhideUi()
{
	mBtnTurns.show();

	mBtnToggleHeightmap.show();
	mBtnToggleConnectedness.show();
	mBtnToggleCommRangeOverlay.show();
	mBtnTogglePoliceOverlay.visible(true);
	mBtnToggleRouteOverlay.show();

	mStructures.show();
	mRobots.show();
	mConnections.show();

	mGameOverDialog.enabled(true);
	mGameOptionsDialog.enabled(true);
}


/**
 * Hides all non-essential UI elements.
 */
void MapViewState::resetUi()
{
	clearMode();
	clearSelections();

	mWindowStack.hide();
}


void MapViewState::clearSelections()
{
	mStructures.clearSelection();
	mConnections.clearSelection();
	mRobots.clearSelection();
}


/**
 * Adds selection options to the Structure Menu
 */
void MapViewState::populateStructureMenu()
{
	mStructures.clear();
	mConnections.clear();

	// Above Ground structures only
	if (NAS2D::Utility<StructureManager>::get().count() == 0)
	{
		if (mTileMap->currentDepth() == constants::DepthSurface)
		{
			mStructures.addItem(constants::SeedLander, 0, StructureID::SID_SEED_LANDER);
		}
	}
	else if (mTileMap->currentDepth() == constants::DepthSurface)
	{
		mStructures.addItem(constants::Agridome, 5, StructureID::SID_AGRIDOME);
		mStructures.addItem(constants::Chap, 3, StructureID::SID_CHAP);
		mStructures.addItem(constants::CommTower, 22, StructureID::SID_COMM_TOWER);
		mStructures.addItem(constants::FusionReactor, 21, StructureID::SID_FUSION_REACTOR);
		mStructures.addItem(constants::HotLaboratory, 18, StructureID::SID_HOT_LABORATORY);
		mStructures.addItem(constants::MaintenanceFacility, 54, StructureID::SID_MAINTENANCE_FACILITY);
		mStructures.addItem(constants::Recycling, 16, StructureID::SID_RECYCLING);
		mStructures.addItem(constants::Road, 24, StructureID::SID_ROAD);
		mStructures.addItem(constants::RobotCommand, 14, StructureID::SID_ROBOT_COMMAND);
		mStructures.addItem(constants::Smelter, 4, StructureID::SID_SMELTER);
		mStructures.addItem(constants::SolarPanel1, 33, StructureID::SID_SOLAR_PANEL1);
		mStructures.addItem(constants::SolarPlant, 10, StructureID::SID_SOLAR_PLANT);
		mStructures.addItem(constants::StorageTanks, 8, StructureID::SID_STORAGE_TANKS);
		mStructures.addItem(constants::SurfaceFactory, 11, StructureID::SID_SURFACE_FACTORY);
		mStructures.addItem(constants::SurfacePolice, 23, StructureID::SID_SURFACE_POLICE);
		mStructures.addItem(constants::Warehouse, 9, StructureID::SID_WAREHOUSE);

		mConnections.addItem(constants::AgTubeIntersection, 110, ConnectorDir::CONNECTOR_INTERSECTION);
		mConnections.addItem(constants::AgTubeRight, 112, ConnectorDir::CONNECTOR_RIGHT);
		mConnections.addItem(constants::AgTubeLeft, 111, ConnectorDir::CONNECTOR_LEFT);

		// Special case code, not thrilled with this
		if (mLandersColonist > 0) { mStructures.addItem(constants::ColonistLander, 2, StructureID::SID_COLONIST_LANDER); }
		if (mLandersCargo > 0) { mStructures.addItem(constants::CargoLander, 1, StructureID::SID_CARGO_LANDER); }
	}
	else
	{
		mStructures.addItem(constants::Laboratory, 58, StructureID::SID_LABORATORY);
		mStructures.addItem(constants::Park, 75, StructureID::SID_PARK);
		mStructures.addItem(constants::UndergroundPolice, 61, StructureID::SID_UNDERGROUND_POLICE);
		mStructures.addItem(constants::RecreationCenter, 73, StructureID::SID_RECREATION_CENTER);
		mStructures.addItem(constants::Residence, 55, StructureID::SID_RESIDENCE);
		mStructures.addItem(constants::UndergroundFactory, 69, StructureID::SID_UNDERGROUND_FACTORY);
		mStructures.addItem(constants::MedicalCenter, 62, StructureID::SID_MEDICAL_CENTER);
		mStructures.addItem(constants::Nursery, 77, StructureID::SID_NURSERY);
		mStructures.addItem(constants::Commercial, 66, StructureID::SID_COMMERCIAL);
		mStructures.addItem(constants::RedLightDistrict, 76, StructureID::SID_RED_LIGHT_DISTRICT);
		mStructures.addItem(constants::University, 63, StructureID::SID_UNIVERSITY);

		mConnections.addItem(constants::UgTubeIntersection, 113, ConnectorDir::CONNECTOR_INTERSECTION);
		mConnections.addItem(constants::UgTubeRight, 115, ConnectorDir::CONNECTOR_RIGHT);
		mConnections.addItem(constants::UgTubelLeft, 114, ConnectorDir::CONNECTOR_LEFT);
	}

	updateStructuresAvailability();

	mStructures.sort();
}


/**
 * Some UI elements should be treated as 'modal' and block input when they're
 * visible. This is a central way to check for a modal UI element without
 * having to check for multiple elements in a variety of places in the code.
 */
bool MapViewState::modalUiElementDisplayed() const
{
	return mGameOptionsDialog.visible() ||
		mFileIoDialog.visible() ||
		mGameOverDialog.visible();
}


/**
* Updates and draws the UI.
*/
void MapViewState::drawUI()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// Bottom UI
	renderer.drawBoxFilled(mBottomUiRect, NAS2D::Color{39, 39, 39});
	renderer.drawBox(mBottomUiRect, NAS2D::Color{21, 21, 21});
	renderer.drawLine(NAS2D::Point{mBottomUiRect.x + 1, mBottomUiRect.y}, NAS2D::Point{mBottomUiRect.x + mBottomUiRect.width - 2, mBottomUiRect.y}, NAS2D::Color{56, 56, 56});

	drawMiniMap();
	drawResourceInfo();
	drawNavInfo();
	drawRobotInfo();

	mNotificationArea.update();

	// Buttons
	mBtnTurns.update();
	mBtnToggleHeightmap.update();
	mBtnToggleConnectedness.update();
	mBtnToggleCommRangeOverlay.update();
	mBtnToggleRouteOverlay.update();
	mBtnTogglePoliceOverlay.update();

	// Menus
	mRobots.update();
	mStructures.update();
	mConnections.update();

	// Windows
	mFileIoDialog.update();
	mGameOptionsDialog.update();
	mWindowStack.update();

	if (!modalUiElementDisplayed()) { mToolTip.update(); }
}


void MapViewState::setOverlay(TileList& tileList, Tile::Overlay overlay)
{
	for (auto tile : tileList)
	{
		tile->overlay(overlay);
	}
}


void MapViewState::clearOverlays()
{
	clearOverlay(mConnectednessOverlay);
	clearOverlay(mCommRangeOverlay);
	clearOverlay(mPoliceOverlays[mTileMap->currentDepth()]);
	clearOverlay(mTruckRouteOverlay);
}


void MapViewState::clearOverlay(TileList& tileList)
{
	setOverlay(tileList, Tile::Overlay::None);
}


void MapViewState::changePoliceOverlayDepth(int oldDepth, int newDepth)
{
	clearOverlay(mPoliceOverlays[oldDepth]);
	setOverlay(mPoliceOverlays[newDepth], Tile::Overlay::Police);
}


/**
 * Handles clicks of the Connectedness Overlay button.
 */
void MapViewState::onToggleConnectedness()
{
	clearOverlays();

	if (mBtnToggleConnectedness.toggled())
	{
		mBtnToggleCommRangeOverlay.toggle(false);
		mBtnToggleRouteOverlay.toggle(false);
		mBtnTogglePoliceOverlay.toggle(false);

		setOverlay(mConnectednessOverlay, Tile::Overlay::Connectedness);
	}
}


void MapViewState::onToggleCommRangeOverlay()
{
	clearOverlays();

	if (mBtnToggleCommRangeOverlay.toggled())
	{
		mBtnToggleConnectedness.toggle(false);
		mBtnToggleRouteOverlay.toggle(false);
		mBtnTogglePoliceOverlay.toggle(false);

		setOverlay(mCommRangeOverlay, Tile::Overlay::Communications);
	}
}

void MapViewState::onTogglePoliceOverlay()
{
	clearOverlays();

	if (mBtnTogglePoliceOverlay.toggled())
	{
		mBtnToggleCommRangeOverlay.toggle(false);
		mBtnToggleConnectedness.toggle(false);
		mBtnToggleRouteOverlay.toggle(false);

		setOverlay(mPoliceOverlays[mTileMap->currentDepth()], Tile::Overlay::Police);
	}
}

void MapViewState::onToggleRouteOverlay()
{
	clearOverlays();

	if (mBtnToggleRouteOverlay.toggled())
	{
		mBtnToggleConnectedness.toggle(false);
		mBtnToggleCommRangeOverlay.toggle(false);
		mBtnTogglePoliceOverlay.toggle(false);

		setOverlay(mTruckRouteOverlay, Tile::Overlay::TruckingRoutes);
	}
}


void MapViewState::onNotificationClicked(const NotificationArea::Notification& notification)
{
	mNotificationWindow.notification(notification);
	mNotificationWindow.show();
	mWindowStack.bringToFront(&mNotificationWindow);
}


/**
* Currently uses a text comparison function. Not inherently bad but
* should really be turned into a key/value pair table for easier lookups.
*/
void MapViewState::onStructuresSelectionChange(const IconGrid::IconGridItem* item)
{
	mConnections.clearSelection();
	mRobots.clearSelection();

	if (!item) { return; }

	// Check availability
	if (!item->available)
	{
		resourceShortageMessage(mResourcesCount, static_cast<StructureID>(item->meta));
		mStructures.clearSelection();
		return;
	}

	setStructureID(static_cast<StructureID>(item->meta), InsertMode::Structure);
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void MapViewState::onConnectionsSelectionChange(const IconGrid::IconGridItem* /*item*/)
{
	mRobots.clearSelection();
	mStructures.clearSelection();

	setStructureID(StructureID::SID_TUBE, InsertMode::Tube);
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void MapViewState::onRobotsSelectionChange(const IconGrid::IconGridItem* item)
{
	mConnections.clearSelection();
	mStructures.clearSelection();

	if (!item)
	{
		clearMode();
		return;
	}

	mCurrentRobot = static_cast<Robot::Type>(item->meta);

	mInsertMode = InsertMode::Robot;
	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(PointerType::POINTER_PLACE_TILE);
}


void MapViewState::onDiggerSelectionDialog(Direction direction, Tile* tile)
{
	// Before doing anything, if we're going down and the depth is not the surface,
	// the assumption is that we've already checked and determined that there's an air shaft
	// so clear it from the tile, disconnect the tile and run a connectedness search.
	if (tile->depth() > 0 && direction == Direction::Down)
	{
		NAS2D::Utility<StructureManager>::get().removeStructure(tile->structure());
		NAS2D::Utility<StructureManager>::get().disconnectAll();
		tile->deleteThing();
		tile->connected(false);
		checkConnectedness();
	}

	// Assumes a digger is available.
	Robodigger& robot = mRobotPool.getDigger();
	robot.startTask(static_cast<int>(tile->index()) + constants::DiggerTaskTime);
	mRobotPool.insertRobotIntoTable(mRobotList, &robot, tile);

	robot.direction(direction);

	const auto directionOffset = directionEnumToOffset(direction);
	if (directionOffset != DirectionCenter)
	{
		mTileMap->getTile({tile->xy() + directionOffset, tile->depth()}).excavated(true);
	}

	if (!mRobotPool.robotAvailable(Robot::Type::Digger))
	{
		mRobots.removeItem(constants::Robodigger);
		clearMode();
	}

	mDiggerDirection.visible(false);
}


/**
 * Click handler for the main menu Save Game button.
 */
void MapViewState::onSaveGame()
{
	mGameOptionsDialog.hide();
	mFileIoDialog.scanDirectory(constants::SaveGamePath);
	mFileIoDialog.setMode(FileIo::FileOperation::Save);
	mFileIoDialog.show();
}


/**
 * Click handler for the main menu Load Game button.
 */
void MapViewState::onLoadGame()
{
	mGameOptionsDialog.hide();
	mFileIoDialog.scanDirectory(constants::SaveGamePath);
	mFileIoDialog.setMode(FileIo::FileOperation::Load);
	mFileIoDialog.show();

}


/**
 * Click handler for the main menu Return to Game button.
 */
void MapViewState::onReturnToGame()
{
	mGameOptionsDialog.hide();
}


/**
 * Click handler for the main menu Return to Main Menu Screen button.
 */
void MapViewState::onGameOver()
{
	NAS2D::Utility<NAS2D::Renderer>::get().fadeOut(constants::FadeSpeed);
	mQuitSignal();
}


/**
 * Handler for File I/O actions.
 */
void MapViewState::onFileIoAction(const std::string& filePath, FileIo::FileOperation fileOp)
{
	if (fileOp == FileIo::FileOperation::Load)
	{
		try
		{
			load(constants::SaveGamePath + filePath + ".xml");
		}
		catch (const std::exception& e)
		{
			doNonFatalErrorMessage("Load Failed", e.what());
			return;
		}
	}
	else
	{
		save(constants::SaveGamePath + filePath + ".xml");
	}

	mFileIoDialog.hide();
}


void MapViewState::onNotificationWindowTakeMeThere(const MapCoordinate& position)
{
	mTileMap->centerMapOnTile(&mTileMap->getTile(position.xy));
	mTileMap->currentDepth(position.z);
}


/**
 * Turns button clicked.
 */
void MapViewState::onTurns()
{
	nextTurn();
}


/**
 * Update IconGridItems availability
 */
void MapViewState::updateStructuresAvailability()
{
	for (int sid = 1; sid < StructureID::SID_COUNT; ++sid)
	{
		const StructureID id = static_cast<StructureID>(sid);
		mStructures.itemAvailable(StructureName(id), StructureCatalogue::canBuild(mResourcesCount, id));
	}
}
