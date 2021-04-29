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
#include "../Map/TileMap.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <cmath>


using namespace constants;


static void setOverlay(Button& button, TileList& tileList, Tile::Overlay overlay)
{
	auto overlayToUse = button.toggled() ? overlay : Tile::Overlay::None;
	for (auto tile : tileList)
	{
		tile->overlay(overlayToUse);
	}
}


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

	mRobotInspector.position(renderer.center() - NAS2D::Vector{ mRobotInspector.size().x / 2.0f, 175.0f });
	mRobotInspector.hide();

	mFactoryProduction.position(NAS2D::Point{renderer.center().x - mFactoryProduction.size().x / 2.0f, 175.0f});
	mFactoryProduction.hide();

	mFileIoDialog.setMode(FileIo::FileOperation::Save);
	mFileIoDialog.fileOperation().connect(this, &MapViewState::onFileIoAction);
	mFileIoDialog.anchored(true);
	mFileIoDialog.hide();

	mPopulationPanel.position({675, constants::RESOURCE_ICON_SIZE + 4 + constants::MARGIN_TIGHT});
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

	const auto size = renderer.size().to<int>();
	mBottomUiRect = {0, size.y - constants::BOTTOM_UI_HEIGHT, size.x, constants::BOTTOM_UI_HEIGHT};

	// BUTTONS
	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.position(NAS2D::Point{mMiniMapBoundingBox.x - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT, size.y - constants::MARGIN - MAIN_BUTTON_SIZE});
	mBtnTurns.size(constants::MAIN_BUTTON_SIZE);
	mBtnTurns.click().connect(this, &MapViewState::onTurns);
	mBtnTurns.enabled(false);

	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleHeightmap.type(Button::Type::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleConnectedness.type(Button::Type::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().connect(this, &MapViewState::onToggleConnectedness);

	mBtnToggleCommRangeOverlay.image("ui/icons/comm_overlay.png");
	mBtnToggleCommRangeOverlay.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleCommRangeOverlay.type(Button::Type::BUTTON_TOGGLE);
	mBtnToggleCommRangeOverlay.click().connect(this, &MapViewState::onToggleCommRangeOverlay);

	mBtnToggleRouteOverlay.image("ui/icons/route.png");
	mBtnToggleRouteOverlay.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleRouteOverlay.type(Button::Type::BUTTON_TOGGLE);
	mBtnToggleRouteOverlay.click().connect(this, &MapViewState::onToggleRouteOverlay);

	// Menus
	mRobots.position({mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52, mBottomUiRect.y + MARGIN});
	mRobots.size({52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2});
	mRobots.showTooltip(true);
	mRobots.selectionChanged().connect(this, &MapViewState::onRobotsSelectionChange);

	mConnections.position({mRobots.positionX() - constants::MARGIN_TIGHT - 52, mBottomUiRect.y + MARGIN});
	mConnections.size({52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2});
	mConnections.selectionChanged().connect(this, &MapViewState::onConnectionsSelectionChange);

	mStructures.position(NAS2D::Point{constants::MARGIN, mBottomUiRect.y + MARGIN});
	mStructures.size({mConnections.positionX() - constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2});
	mStructures.showTooltip(true);
	mStructures.selectionChanged().connect(this, &MapViewState::onStructuresSelectionChange);

	// Initial Structures
	mStructures.addItem(constants::SEED_LANDER, 0, StructureID::SID_SEED_LANDER);


	// tooltip control sizes
	constexpr auto hudHeight = constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2;
	mTooltipResourceBreakdown.size({ 265, hudHeight });

	mTooltipResourceStorage.position({ 275, 0 });
	mTooltipResourceStorage.size({ 85, hudHeight });

	mTooltipFoodStorage.position({ 420, 0 });
	mTooltipFoodStorage.size({ 75, hudHeight });

	mTooltipEnergy.position({ 560, 0 });
	mTooltipEnergy.size({ 55, hudHeight });

	mTooltipPopulation.position({ 670, 0 });
	mTooltipPopulation.size({ 75, hudHeight });

	mTooltipCurrentTurns.size({ 45, hudHeight });
	mTooltipSystemButton.size({ hudHeight, hudHeight });

	// Tool Tips
	mToolTip.add(mBtnTurns, constants::ToolTipBtnTurns);
	mToolTip.add(mBtnToggleHeightmap, constants::ToolTipBtnHeightmap);
	mToolTip.add(mBtnToggleConnectedness, constants::ToolTipBtnConnectedness);
	mToolTip.add(mBtnToggleCommRangeOverlay, constants::ToolTipBtnCommRange);
	mToolTip.add(mBtnToggleRouteOverlay, constants::ToolTipBtnRoutes);
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
	mBottomUiRect = {0, size.y - constants::BOTTOM_UI_HEIGHT, size.x, constants::BOTTOM_UI_HEIGHT};

	// Menu / System Icon
	mTooltipSystemButton.position({ size.x - (constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2), 0 });
	mTooltipCurrentTurns.position({ size.x - 80 , 0 });

	// NAVIGATION BUTTONS
	// Bottom line
	const auto navIconSpacing = 32 + constants::MARGIN_TIGHT;
	mMoveDownIconRect = {size.x - navIconSpacing, size.y - constants::BOTTOM_UI_HEIGHT - 65, 32, 32};
	mMoveEastIconRect = {mMoveDownIconRect.x - navIconSpacing, mMoveDownIconRect.y + 8, 32, 16};
	mMoveSouthIconRect = {mMoveDownIconRect.x - 2 * navIconSpacing, mMoveDownIconRect.y + 8, 32, 16};

	// Top line
	mMoveUpIconRect = {mMoveDownIconRect.x, mMoveDownIconRect.y - navIconSpacing, 32, 32};
	mMoveNorthIconRect = {mMoveUpIconRect.x - navIconSpacing, mMoveUpIconRect.y + 8, 32, 16};
	mMoveWestIconRect = {mMoveUpIconRect.x - 2 * navIconSpacing, mMoveUpIconRect.y + 8, 32, 16};

	// Mini Map
	mMiniMapBoundingBox = {size.x - 300 - constants::MARGIN, mBottomUiRect.y + constants::MARGIN, 300, 150};

	// Position UI Buttons
	mBtnTurns.position(NAS2D::Point{mMiniMapBoundingBox.x - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT, size.y - constants::MARGIN - MAIN_BUTTON_SIZE});
	mBtnToggleHeightmap.position({mBtnTurns.positionX(), mMiniMapBoundingBox.y});
	mBtnToggleConnectedness.position({ mBtnTurns.positionX(), mMiniMapBoundingBox.y + constants::MAIN_BUTTON_SIZE });
	mBtnToggleCommRangeOverlay.position({ mBtnTurns.positionX(), mMiniMapBoundingBox.y + (constants::MAIN_BUTTON_SIZE * 2) });
	mBtnToggleRouteOverlay.position({ mBtnTurns.positionX(), mMiniMapBoundingBox.y + (constants::MAIN_BUTTON_SIZE * 3) });

	// UI Panels
	mRobots.position({mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52, mBottomUiRect.y + MARGIN});
	mConnections.position({mRobots.positionX() - constants::MARGIN_TIGHT - 52, mBottomUiRect.y + MARGIN});
	mStructures.position(NAS2D::Point{constants::MARGIN, mBottomUiRect.y + MARGIN});

	mStructures.size({mConnections.positionX() - constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2});

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
	mBtnTurns.visible(true);

	mBtnToggleHeightmap.visible(true);
	mBtnToggleConnectedness.visible(true);
	mBtnToggleCommRangeOverlay.visible(true);
	mBtnToggleRouteOverlay.show();

	mStructures.visible(true);
	mRobots.visible(true);
	mConnections.visible(true);

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
		if (mTileMap->currentDepth() == constants::DEPTH_SURFACE)
		{
			mStructures.addItem(constants::SEED_LANDER, 0, StructureID::SID_SEED_LANDER);
		}
	}
	else if (mTileMap->currentDepth() == constants::DEPTH_SURFACE)
	{
		mStructures.addItem(constants::AGRIDOME, 5, StructureID::SID_AGRIDOME);
		mStructures.addItem(constants::CHAP, 3, StructureID::SID_CHAP);
		mStructures.addItem(constants::FUSION_REACTOR, 21, StructureID::SID_FUSION_REACTOR);
		mStructures.addItem(constants::HOT_LABORATORY, 18, StructureID::SID_HOT_LABORATORY);
		mStructures.addItem(constants::ROBOT_COMMAND, 14, StructureID::SID_ROBOT_COMMAND);
		mStructures.addItem(constants::COMM_TOWER, 22, StructureID::SID_COMM_TOWER);
		mStructures.addItem(constants::RECYCLING, 16, StructureID::SID_RECYCLING);
		mStructures.addItem(constants::ROAD, 24, StructureID::SID_ROAD);
		mStructures.addItem(constants::SURFACE_POLICE, 23, StructureID::SID_SURFACE_POLICE);
		mStructures.addItem(constants::SMELTER, 4, StructureID::SID_SMELTER);
		mStructures.addItem(constants::SOLAR_PANEL1, 33, StructureID::SID_SOLAR_PANEL1);
		mStructures.addItem(constants::SOLAR_PLANT, 10, StructureID::SID_SOLAR_PLANT);
		mStructures.addItem(constants::STORAGE_TANKS, 8, StructureID::SID_STORAGE_TANKS);
		mStructures.addItem(constants::SURFACE_FACTORY, 11, StructureID::SID_SURFACE_FACTORY);
		mStructures.addItem(constants::WAREHOUSE, 9, StructureID::SID_WAREHOUSE);

		mConnections.addItem(constants::AG_TUBE_INTERSECTION, 110, ConnectorDir::CONNECTOR_INTERSECTION);
		mConnections.addItem(constants::AG_TUBE_RIGHT, 112, ConnectorDir::CONNECTOR_RIGHT);
		mConnections.addItem(constants::AG_TUBE_LEFT, 111, ConnectorDir::CONNECTOR_LEFT);

		// Special case code, not thrilled with this
		if (mLandersColonist > 0) { mStructures.addItem(constants::COLONIST_LANDER, 2, StructureID::SID_COLONIST_LANDER); }
		if (mLandersCargo > 0) { mStructures.addItem(constants::CARGO_LANDER, 1, StructureID::SID_CARGO_LANDER); }
	}
	else
	{
		mStructures.addItem(constants::LABORATORY, 58, StructureID::SID_LABORATORY);
		mStructures.addItem(constants::PARK, 75, StructureID::SID_PARK);
		mStructures.addItem(constants::UNDERGROUND_POLICE, 61, StructureID::SID_UNDERGROUND_POLICE);
		mStructures.addItem(constants::RECREATION_CENTER, 73, StructureID::SID_RECREATION_CENTER);
		mStructures.addItem(constants::RESIDENCE, 55, StructureID::SID_RESIDENCE);
		mStructures.addItem(constants::UNDERGROUND_FACTORY, 69, StructureID::SID_UNDERGROUND_FACTORY);
		mStructures.addItem(constants::MEDICAL_CENTER, 62, StructureID::SID_MEDICAL_CENTER);
		mStructures.addItem(constants::NURSERY, 77, StructureID::SID_NURSERY);
		mStructures.addItem(constants::COMMERCIAL, 66, StructureID::SID_COMMERCIAL);
		mStructures.addItem(constants::RED_LIGHT_DISTRICT, 76, StructureID::SID_RED_LIGHT_DISTRICT);
		mStructures.addItem(constants::UNIVERSITY, 63, StructureID::SID_UNIVERSITY);

		mConnections.addItem(constants::UG_TUBE_INTERSECTION, 113, ConnectorDir::CONNECTOR_INTERSECTION);
		mConnections.addItem(constants::UG_TUBE_RIGHT, 115, ConnectorDir::CONNECTOR_RIGHT);
		mConnections.addItem(constants::UG_TUBE_LEFT, 114, ConnectorDir::CONNECTOR_LEFT);
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
	renderer.drawBoxFilled(mBottomUiRect, NAS2D::Color{ 39, 39, 39 });
	renderer.drawBox(mBottomUiRect, NAS2D::Color{ 21, 21, 21 });
	renderer.drawLine(NAS2D::Point{ mBottomUiRect.x + 1, mBottomUiRect.y }, NAS2D::Point{ mBottomUiRect.x + mBottomUiRect.width - 2, mBottomUiRect.y }, NAS2D::Color{ 56, 56, 56 });

	drawMiniMap();
	drawResourceInfo();
	drawNavInfo();
	drawRobotInfo();

	// Buttons
	mBtnTurns.update();
	mBtnToggleHeightmap.update();
	mBtnToggleConnectedness.update();
	mBtnToggleCommRangeOverlay.update();
	mBtnToggleRouteOverlay.update();

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


/**
 * Handles clicks of the Connectedness Overlay button.
 */
void MapViewState::onToggleConnectedness()
{
	if (mBtnToggleConnectedness.toggled())
	{
		mBtnToggleCommRangeOverlay.toggle(false);
		mBtnToggleRouteOverlay.toggle(false);

		onToggleCommRangeOverlay();
		onToggleRouteOverlay();
	}

	setOverlay(mBtnToggleConnectedness, mConnectednessOverlay, Tile::Overlay::Connectedness);
}


void MapViewState::onToggleCommRangeOverlay()
{
	if (mBtnToggleCommRangeOverlay.toggled())
	{
		mBtnToggleConnectedness.toggle(false);
		mBtnToggleRouteOverlay.toggle(false);

		onToggleConnectedness();
		onToggleRouteOverlay();
	}

	setOverlay(mBtnToggleCommRangeOverlay, mCommRangeOverlay, Tile::Overlay::Communications);
}


void MapViewState::onToggleRouteOverlay()
{
	if (mBtnToggleRouteOverlay.toggled())
	{
		mBtnToggleConnectedness.toggle(false);
		mBtnToggleCommRangeOverlay.toggle(false);

		onToggleCommRangeOverlay();
		onToggleConnectedness();
	}

	setOverlay(mBtnToggleRouteOverlay, mTruckRouteOverlay, Tile::Overlay::TruckingRoutes);
}


/**
* Currently uses a text comparison function. Not inherently bad but
* should really be turned into a key/value pair table for easier lookups.
*/
void MapViewState::onStructuresSelectionChange(const IconGrid::IconGridItem* _item)
{
	mConnections.clearSelection();
	mRobots.clearSelection();

	if (!_item) { return; }

	// Check availability
	if (!_item->available)
	{
		resourceShortageMessage(mResourcesCount, static_cast<StructureID>(_item->meta));
		mStructures.clearSelection();
		return;
	}

	setStructureID(static_cast<StructureID>(_item->meta), InsertMode::Structure);
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void MapViewState::onConnectionsSelectionChange(const IconGrid::IconGridItem* /*_item*/)
{
	mRobots.clearSelection();
	mStructures.clearSelection();

	setStructureID(StructureID::SID_TUBE, InsertMode::Tube);
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void MapViewState::onRobotsSelectionChange(const IconGrid::IconGridItem* _item)
{
	mConnections.clearSelection();
	mStructures.clearSelection();

	if (!_item)
	{
		clearMode();
		return;
	}

	mCurrentRobot = static_cast<Robot::Type>(_item->meta);

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
	Robodigger* robot = mRobotPool.getDigger();
	robot->startTask(static_cast<int>(tile->index()) + DIGGER_TASK_TIME);
	mRobotPool.insertRobotIntoTable(mRobotList, robot, tile);

	robot->direction(direction);


	if (direction == Direction::North)
	{
		mTileMap->getTile(tile->position() + DirectionNorth, tile->depth()).excavated(true);
	}
	else if (direction == Direction::South)
	{
		mTileMap->getTile(tile->position() + DirectionSouth, tile->depth()).excavated(true);
	}
	else if (direction == Direction::East)
	{
		mTileMap->getTile(tile->position() + DirectionEast, tile->depth()).excavated(true);
	}
	else if (direction == Direction::West)
	{
		mTileMap->getTile(tile->position() + DirectionWest, tile->depth()).excavated(true);
	}


	if (!mRobotPool.robotAvailable(Robot::Type::Digger))
	{
		mRobots.removeItem(constants::ROBODIGGER);
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
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.setMode(FileIo::FileOperation::Save);
	mFileIoDialog.show();
}


/**
 * Click handler for the main menu Load Game button.
 */
void MapViewState::onLoadGame()
{
	mGameOptionsDialog.hide();
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
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
	NAS2D::Utility<NAS2D::Renderer>::get().fadeOut(static_cast<float>(constants::FADE_SPEED));
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
			load(constants::SAVE_GAME_PATH + filePath + ".xml");
		}
		catch (const std::exception& e)
		{
			doNonFatalErrorMessage("Load Failed", e.what());
			return;
		}
	}
	else
	{
		save(constants::SAVE_GAME_PATH + filePath + ".xml");
	}

	mFileIoDialog.hide();
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
