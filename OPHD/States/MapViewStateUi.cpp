// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <cmath>


using namespace constants;

NAS2D::Rectangle<int> BOTTOM_UI_AREA;


/**
 * \fixme	Yuck, not thrilled with this but whatever, it works.
 */
extern NAS2D::Rectangle<int> MENU_ICON;
extern NAS2D::Rectangle<int> MOVE_NORTH_ICON;
extern NAS2D::Rectangle<int> MOVE_SOUTH_ICON;
extern NAS2D::Rectangle<int> MOVE_EAST_ICON;
extern NAS2D::Rectangle<int> MOVE_WEST_ICON;
extern NAS2D::Rectangle<int> MOVE_UP_ICON;
extern NAS2D::Rectangle<int> MOVE_DOWN_ICON;


/**
 * Sets up the user interface elements
 * 
 * \note	The explicit casts to int to truncate floating point values to force
 *			window positions to a whole number.
 */
void MapViewState::initUi()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	mDiggerDirection.directionSelected().connect(this, &MapViewState::diggerSelectionDialog);
	mDiggerDirection.hide();

	mTileInspector.position(renderer.center() - NAS2D::Vector{mTileInspector.size().x / 2.0f, 175.0f});
	mTileInspector.hide();

	mStructureInspector.position(renderer.center() - NAS2D::Vector{mStructureInspector.size().x / 2.0f, 175.0f});
	mStructureInspector.hide();

	mFactoryProduction.position(NAS2D::Point{renderer.center().x - mFactoryProduction.size().x / 2.0f, 175.0f});
	mFactoryProduction.hide();

	mFileIoDialog.setMode(FileIo::FileOperation::FILE_SAVE);
	mFileIoDialog.fileOperation().connect(this, &MapViewState::fileIoAction);
	mFileIoDialog.anchored(true);
	mFileIoDialog.hide();

	mPopulationPanel.position({675, constants::RESOURCE_ICON_SIZE + 4 + constants::MARGIN_TIGHT});
	mPopulationPanel.population(&mPopulation);
	mPopulationPanel.morale(&mCurrentMorale);
	mPopulationPanel.old_morale(&mPreviousMorale);

	mResourceBreakdownPanel.position({0, 22});
	mResourceBreakdownPanel.playerResources(&mPlayerResources);

	mGameOverDialog.returnToMainMenu().connect(this, &MapViewState::btnGameOverClicked);
	mGameOverDialog.hide();

	mGameOptionsDialog.SaveGame().connect(this, &MapViewState::btnSaveGameClicked);
	mGameOptionsDialog.LoadGame().connect(this, &MapViewState::btnLoadGameClicked);
	mGameOptionsDialog.returnToGame().connect(this, &MapViewState::btnReturnToGameClicked);
	mGameOptionsDialog.returnToMainMenu().connect(this, &MapViewState::btnGameOverClicked);
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

	const auto size = renderer.size().to<int>();
	BOTTOM_UI_AREA = {0, size.y - constants::BOTTOM_UI_HEIGHT, size.x, constants::BOTTOM_UI_HEIGHT};

	// BUTTONS
	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.position(NAS2D::Point{mMiniMapBoundingBox.x - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT, size.y - constants::MARGIN - MAIN_BUTTON_SIZE});
	mBtnTurns.size(constants::MAIN_BUTTON_SIZE);
	mBtnTurns.click().connect(this, &MapViewState::btnTurnsClicked);
	mBtnTurns.enabled(false);

	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleHeightmap.type(Button::Type::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleConnectedness.type(Button::Type::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().connect(this, &MapViewState::btnToggleConnectednessClicked);

	// Menus
	mRobots.position({mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52, BOTTOM_UI_AREA.y + MARGIN});
	mRobots.size({52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2});
	mRobots.showTooltip(true);
	mRobots.selectionChanged().connect(this, &MapViewState::robotsSelectionChanged);

	mConnections.position({mRobots.positionX() - constants::MARGIN_TIGHT - 52, BOTTOM_UI_AREA.y + MARGIN});
	mConnections.size({52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2});
	mConnections.selectionChanged().connect(this, &MapViewState::connectionsSelectionChanged);

	mStructures.position(NAS2D::Point{constants::MARGIN, BOTTOM_UI_AREA.y + MARGIN});
	mStructures.size({mConnections.positionX() - constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2});
	mStructures.showTooltip(true);
	mStructures.selectionChanged().connect(this, &MapViewState::structuresSelectionChanged);

	// Initial Structures
	mStructures.addItem(constants::SEED_LANDER, 0, StructureID::SID_SEED_LANDER);
}


void MapViewState::setupUiPositions(NAS2D::Vector<int> size)
{
	// Bottom UI Area
	BOTTOM_UI_AREA = {0, size.y - constants::BOTTOM_UI_HEIGHT, size.x, constants::BOTTOM_UI_HEIGHT};

	// Menu / System Icon
	const auto menuIconSpacing = constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2;
	MENU_ICON = {size.x - menuIconSpacing, 0, menuIconSpacing, menuIconSpacing};

	// NAVIGATION BUTTONS
	// Bottom line
	const auto navIconSpacing = 32 + constants::MARGIN_TIGHT;
	MOVE_DOWN_ICON = {size.x - navIconSpacing, size.y - constants::BOTTOM_UI_HEIGHT - 65, 32, 32};
	MOVE_EAST_ICON = {MOVE_DOWN_ICON.x - navIconSpacing, MOVE_DOWN_ICON.y + 8, 32, 16};
	MOVE_SOUTH_ICON = {MOVE_DOWN_ICON.x - 2 * navIconSpacing, MOVE_DOWN_ICON.y + 8, 32, 16};

	// Top line
	MOVE_UP_ICON = {MOVE_DOWN_ICON.x, MOVE_DOWN_ICON.y - navIconSpacing, 32, 32};
	MOVE_NORTH_ICON = {MOVE_UP_ICON.x - navIconSpacing, MOVE_UP_ICON.y + 8, 32, 16};
	MOVE_WEST_ICON = {MOVE_UP_ICON.x - 2 * navIconSpacing, MOVE_UP_ICON.y + 8, 32, 16};

	// Mini Map
	mMiniMapBoundingBox = {size.x - 300 - constants::MARGIN, BOTTOM_UI_AREA.y + constants::MARGIN, 300, 150};

	// Position UI Buttons
	mBtnTurns.position(NAS2D::Point{mMiniMapBoundingBox.x - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT, size.y - constants::MARGIN - MAIN_BUTTON_SIZE});
	mBtnToggleHeightmap.position({mBtnTurns.positionX(), mMiniMapBoundingBox.y});
	mBtnToggleConnectedness.position({mBtnTurns.positionX(), mMiniMapBoundingBox.y + constants::MAIN_BUTTON_SIZE + constants::MARGIN_TIGHT});

	// UI Panels
	mRobots.position({mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52, BOTTOM_UI_AREA.y + MARGIN});
	mConnections.position({mRobots.positionX() - constants::MARGIN_TIGHT - 52, BOTTOM_UI_AREA.y + MARGIN});
	mStructures.position(NAS2D::Point{constants::MARGIN, BOTTOM_UI_AREA.y + MARGIN});

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

	mStructures.hide();
	mRobots.hide();
	mConnections.hide();

	mWindowStack.hide();
}



/**
 * Unhide's the UI.
 */
void MapViewState::unhideUi()
{
	mBtnTurns.visible(true);

	mBtnToggleHeightmap.visible(true);
	mBtnToggleConnectedness.visible(true);

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
	mStructures.dropAllItems();
	mConnections.dropAllItems();

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
	renderer.drawBoxFilled(BOTTOM_UI_AREA, NAS2D::Color{39, 39, 39});
	renderer.drawBox(BOTTOM_UI_AREA, NAS2D::Color{21, 21, 21});
	renderer.drawLine(NAS2D::Point{BOTTOM_UI_AREA.x + 1, BOTTOM_UI_AREA.y}, NAS2D::Point{BOTTOM_UI_AREA.x + BOTTOM_UI_AREA.width - 2, BOTTOM_UI_AREA.y}, NAS2D::Color{56, 56, 56});

	drawMiniMap();
	drawResourceInfo();
	drawNavInfo();
	drawRobotInfo();

	// Buttons
	mBtnToggleHeightmap.update();
	mBtnToggleConnectedness.update();

	mBtnTurns.update();

	// Menus
	mRobots.update();
	mStructures.update();
	mConnections.update();

	// Windows
	mFileIoDialog.update();
	mGameOptionsDialog.update();
	mWindowStack.update();
}


/**
 * Handles clicks of the Connectedness Overlay button.
 */
void MapViewState::btnToggleConnectednessClicked()
{
	mTileMap->toggleShowConnections();
}


/**
* Currently uses a text comparison function. Not inherently bad but
* should really be turned into a key/value pair table for easier lookups.
*/
void MapViewState::structuresSelectionChanged(const IconGrid::IconGridItem* _item)
{
	mConnections.clearSelection();
	mRobots.clearSelection();

	if (!_item) { return; }

	// Check availability
	if (!_item->available)
	{
		resourceShortageMessage(mPlayerResources, static_cast<StructureID>(_item->meta));
		mStructures.clearSelection();
		return;
	}

	setStructureID(static_cast<StructureID>(_item->meta), InsertMode::Structure);
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void MapViewState::connectionsSelectionChanged(const IconGrid::IconGridItem* /*_item*/)
{
	mRobots.clearSelection();
	mStructures.clearSelection();

	setStructureID(StructureID::SID_TUBE, InsertMode::Tube);
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void MapViewState::robotsSelectionChanged(const IconGrid::IconGridItem* _item)
{
	mConnections.clearSelection();
	mStructures.clearSelection();

	if (!_item)
	{
		clearMode();
		return;
	}

	mCurrentRobot = static_cast<RobotType>(_item->meta);

	mInsertMode = InsertMode::Robot;
	NAS2D::Utility<NAS2D::Renderer>::get().setCursor(PointerType::POINTER_PLACE_TILE);
}


/**
 * 
 */
void MapViewState::diggerSelectionDialog(Direction direction, Tile* tile)
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
	robot->startTask(tile->index() + DIGGER_TASK_TIME);
	mRobotPool.insertRobotIntoTable(mRobotList, robot, tile);

	robot->direction(direction);


	if (direction == Direction::North)
	{
		mTileMap->getTile(tile->position() + DirectionNorth, tile->depth())->excavated(true);
	}
	else if (direction == Direction::South)
	{
		mTileMap->getTile(tile->position() + DirectionSouth, tile->depth())->excavated(true);
	}
	else if (direction == Direction::East)
	{
		mTileMap->getTile(tile->position() + DirectionEast, tile->depth())->excavated(true);
	}
	else if (direction == Direction::West)
	{
		mTileMap->getTile(tile->position() + DirectionWest, tile->depth())->excavated(true);
	}


	if (!mRobotPool.robotAvailable(RobotType::ROBOT_DIGGER))
	{
		mRobots.removeItem(constants::ROBODIGGER);
		clearMode();
	}

	mDiggerDirection.visible(false);
}


/**
 * Click handler for the main menu Save Game button.
 */
void MapViewState::btnSaveGameClicked()
{
	mGameOptionsDialog.hide();
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.setMode(FileIo::FileOperation::FILE_SAVE);
	mFileIoDialog.show();
}


/**
 * Click handler for the main menu Load Game button.
 */
void MapViewState::btnLoadGameClicked()
{
	mGameOptionsDialog.hide();
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.setMode(FileIo::FileOperation::FILE_LOAD);
	mFileIoDialog.show();

}


/**
 * Click handler for the main menu Return to Game button.
 */
void MapViewState::btnReturnToGameClicked()
{
	mGameOptionsDialog.hide();
}


/**
 * Click handler for the main menu Return to Main Menu Screen button.
 */
void MapViewState::btnGameOverClicked()
{
	NAS2D::Utility<NAS2D::Renderer>::get().fadeOut(static_cast<float>(constants::FADE_SPEED));
	mQuitCallback();
}


/**
 * Handler for File I/O actions.
 */
void MapViewState::fileIoAction(const std::string& filePath, FileIo::FileOperation fileOp)
{
	if (fileOp == FileIo::FileOperation::FILE_LOAD)
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
void MapViewState::btnTurnsClicked()
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
		mStructures.itemAvailable(StructureName(id), StructureCatalogue::canBuild(mPlayerResources, id));
	}
}
