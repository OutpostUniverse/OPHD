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

#include "../StructureCatalogue.h"
#include "../StructureTranslator.h"

using namespace constants;

NAS2D::Rectangle_2d	BOTTOM_UI_AREA;


/**
 * \fixme	Yuck, not thrilled with this but whatever, it works.
 */
extern Rectangle_2d MENU_ICON;
extern Rectangle_2d MOVE_NORTH_ICON;
extern Rectangle_2d MOVE_SOUTH_ICON;
extern Rectangle_2d MOVE_EAST_ICON;
extern Rectangle_2d MOVE_WEST_ICON;
extern Rectangle_2d MOVE_UP_ICON;
extern Rectangle_2d MOVE_DOWN_ICON;


extern NAS2D::Image* IMG_LOADING;	/// \fixme Find a sane place for this.
extern NAS2D::Image* IMG_SAVING;	/// \fixme Find a sane place for this.
extern NAS2D::Image* IMG_PROCESSING_TURN;	/// \fixme Find a sane place for this.


/**
 * Performs common computations for window centering and casts the resulting
 * fractional value to an int.
 *
 * \note	Truncating the fractional value is intentional.
 */
static inline int centerWindowWidth(float width)
{
	return static_cast<int>(Utility<Renderer>::get().center_x() - width / 2);
}


/**
 * Performs common computations for window centering and casts the resulting
 * fractional value to an int.
 *
 * \note	Truncating the fractional value is intentional.
 */
static inline int centerWindowHeight(float height)
{
	return static_cast<int>(Utility<Renderer>::get().center_y() - height / 2);
}



/**
 * Sets up the user interface elements
 * 
 * \note	The explicit casts to int to truncate floating point values to force
 *			window positions to a whole number.
 */
void MapViewState::initUi()
{
	Renderer& r = Utility<Renderer>::get();

	mDiggerDirection.directionSelected().connect(this, &MapViewState::diggerSelectionDialog);
	mDiggerDirection.hide();

	mTileInspector.position(r.center_x() - mTileInspector.width() / 2.0f, r.height() / 2.0f - 175.0f);
	mTileInspector.hide();

	mStructureInspector.position(r.center_x() - mStructureInspector.width() / 2.0f, r.height() / 2.0f - 175.0f);
	mStructureInspector.hide();

	mFactoryProduction.position(r.center_x() - mFactoryProduction.width() / 2.0f, 175.0f);
	mFactoryProduction.hide();

	mFileIoDialog.setMode(FileIo::FILE_SAVE);
	mFileIoDialog.fileOperation().connect(this, &MapViewState::fileIoAction);
	mFileIoDialog.anchored(true);
	mFileIoDialog.hide();

	mPopulationPanel.position(675.0f, constants::RESOURCE_ICON_SIZE + 4.0f + constants::MARGIN_TIGHT);
	mPopulationPanel.population(&mPopulation);
	mPopulationPanel.morale(&mCurrentMorale);
	mPopulationPanel.old_morale(&mPreviousMorale);

	mResourceBreakdownPanel.position(0.0f, 22.0f);
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

	BOTTOM_UI_AREA = {0, static_cast<int>(r.height() - constants::BOTTOM_UI_HEIGHT), static_cast<int>(r.width()), constants::BOTTOM_UI_HEIGHT};

	// BUTTONS
	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.position(static_cast<float>(mMiniMapBoundingBox.x() - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT), static_cast<float>(r.height() - constants::MARGIN - MAIN_BUTTON_SIZE));
	mBtnTurns.size(static_cast<float>(constants::MAIN_BUTTON_SIZE));
	mBtnTurns.click().connect(this, &MapViewState::btnTurnsClicked);
	mBtnTurns.enabled(false);

	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(static_cast<float>(constants::MAIN_BUTTON_SIZE));
	mBtnToggleHeightmap.type(Button::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(static_cast<float>(constants::MAIN_BUTTON_SIZE));
	mBtnToggleConnectedness.type(Button::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().connect(this, &MapViewState::btnToggleConnectednessClicked);

	// Menus
	mRobots.sheetPath("ui/robots.png");
	mRobots.position(mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52.0f, static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mRobots.size(52.0f, BOTTOM_UI_HEIGHT - constants::MARGIN * 2.0f);
	mRobots.iconSize(46);
	mRobots.iconMargin(constants::MARGIN_TIGHT);
	mRobots.showTooltip(true);
	mRobots.selectionChanged().connect(this, &MapViewState::robotsSelectionChanged);

	mConnections.sheetPath("ui/structures.png");
	mConnections.position(mRobots.positionX() - constants::MARGIN_TIGHT - 52.0f, BOTTOM_UI_AREA.y() + MARGIN);
	mConnections.size(52.0f, BOTTOM_UI_HEIGHT - constants::MARGIN * 2.0f);
	mConnections.iconSize(46);
	mConnections.iconMargin(constants::MARGIN_TIGHT);
	mConnections.selectionChanged().connect(this, &MapViewState::connectionsSelectionChanged);
	mConnections.sorted(false);

	mStructures.sheetPath("ui/structures.png");
	mStructures.position(constants::MARGIN, BOTTOM_UI_AREA.y() + MARGIN);
	mStructures.size(mConnections.positionX() -  constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2.0f);
	mStructures.iconSize(46);
	mStructures.iconMargin(constants::MARGIN_TIGHT);
	mStructures.showTooltip(true);
	mStructures.selectionChanged().connect(this, &MapViewState::structuresSelectionChanged);

	mPlayerResources.resourceObserver().connect(this, &MapViewState::playerResourcePoolModified);

	// Initial Structures
	mStructures.addItem(constants::SEED_LANDER, 0, SID_SEED_LANDER);
}


void MapViewState::setupUiPositions(int w, int h)
{
	//Renderer& r = Utility<Renderer>::get();

	// Bottom UI Area
	BOTTOM_UI_AREA = {0, static_cast<int>(h - constants::BOTTOM_UI_HEIGHT), static_cast<int>(w), constants::BOTTOM_UI_HEIGHT};

	// Menu / System Icon
	MENU_ICON = {w - constants::MARGIN_TIGHT * 2 - constants::RESOURCE_ICON_SIZE, 0, constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2, constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2};

	// NAVIGATION BUTTONS
	// Bottom line
	MOVE_DOWN_ICON = {w - constants::MARGIN - 32, h - constants::BOTTOM_UI_HEIGHT - 65, 32, 32};
	MOVE_EAST_ICON = {MOVE_DOWN_ICON.x() - (32 + constants::MARGIN_TIGHT), MOVE_DOWN_ICON.y() + 8, 32, 16};
	MOVE_SOUTH_ICON = {MOVE_DOWN_ICON.x() - 2 * (32 + constants::MARGIN_TIGHT), MOVE_DOWN_ICON.y() + 8, 32, 16};

	// Top line
	MOVE_UP_ICON = {MOVE_DOWN_ICON.x(), MOVE_DOWN_ICON.y() - constants::MARGIN_TIGHT - 32, 32, 32};
	MOVE_NORTH_ICON = {MOVE_UP_ICON.x() - (32 + constants::MARGIN_TIGHT), MOVE_UP_ICON.y() + 8, 32, 16};
	MOVE_WEST_ICON = {MOVE_UP_ICON.x() - 2 * (32 + constants::MARGIN_TIGHT), MOVE_UP_ICON.y() + 8, 32, 16};

	// Mini Map
	mMiniMapBoundingBox = {static_cast<int>(w - 300 - constants::MARGIN), static_cast<int>(BOTTOM_UI_AREA.y() + constants::MARGIN), 300, 150};

	// Position UI Buttons
	mBtnTurns.position(static_cast<float>(mMiniMapBoundingBox.x() - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT), static_cast<float>(h - constants::MARGIN - MAIN_BUTTON_SIZE));
	mBtnToggleHeightmap.position(mBtnTurns.positionX(), static_cast<float>(mMiniMapBoundingBox.y()));
	mBtnToggleConnectedness.position(mBtnTurns.positionX(), static_cast<float>(mMiniMapBoundingBox.y() + constants::MAIN_BUTTON_SIZE + constants::MARGIN_TIGHT));

	// UI Panels
	mRobots.position(static_cast<float>(mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mConnections.position(static_cast<float>(mRobots.positionX() - constants::MARGIN_TIGHT - 52), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mStructures.position(static_cast<float>(constants::MARGIN), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));

	mStructures.size(mConnections.positionX() - constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mStructures.iconMargin(constants::MARGIN_TIGHT);

	// Anchored window positions
	mFileIoDialog.position(centerWindowWidth(mFileIoDialog.width()), 50.0f);
	mGameOverDialog.position(centerWindowWidth(mGameOverDialog.width()), centerWindowHeight(mGameOverDialog.height()) - 100.0f);
	mAnnouncement.position(centerWindowWidth(mAnnouncement.width()), centerWindowHeight(mAnnouncement.height()) - 100.0f);
	mGameOptionsDialog.position(centerWindowWidth(mGameOptionsDialog.width()), centerWindowHeight(mGameOptionsDialog.height()) - 100.0f);

	mDiggerDirection.position(centerWindowWidth(mDiggerDirection.width()), static_cast<int>(h / 2.0f) - 125.0f);

	mWarehouseInspector.position(centerWindowWidth(mWarehouseInspector.width()), centerWindowHeight(mWarehouseInspector.height()) - 100.0f);
	mMineOperationsWindow.position(centerWindowWidth(mMineOperationsWindow.width()), centerWindowHeight(mMineOperationsWindow.height()) - 100.0f);

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
	if (Utility<StructureManager>::get().count() == 0)
	{
		if (mTileMap->currentDepth() == constants::DEPTH_SURFACE)
		{
			mStructures.addItem(constants::SEED_LANDER, 0, SID_SEED_LANDER);
		}
	}
	else if (mTileMap->currentDepth() == constants::DEPTH_SURFACE)
	{
		mStructures.addItem(constants::AGRIDOME, 5, SID_AGRIDOME);
		mStructures.addItem(constants::CHAP, 3, SID_CHAP);
		mStructures.addItem(constants::FUSION_REACTOR, 21, SID_FUSION_REACTOR);
		mStructures.addItem(constants::HOT_LABORATORY, 18, SID_HOT_LABORATORY);
		mStructures.addItem(constants::ROBOT_COMMAND, 14, SID_ROBOT_COMMAND);
		mStructures.addItem(constants::COMM_TOWER, 22, SID_COMM_TOWER);
		mStructures.addItem(constants::SURFACE_POLICE, 23, SID_SURFACE_POLICE);
		mStructures.addItem(constants::SMELTER, 4, SID_SMELTER);
		mStructures.addItem(constants::SOLAR_PANEL1, 33, SID_SOLAR_PANEL1);
		mStructures.addItem(constants::SOLAR_PLANT, 10, SID_SOLAR_PLANT);
		mStructures.addItem(constants::STORAGE_TANKS, 8, SID_STORAGE_TANKS);
		mStructures.addItem(constants::SURFACE_FACTORY, 11, SID_SURFACE_FACTORY);
		mStructures.addItem(constants::WAREHOUSE, 9, SID_WAREHOUSE);

		mConnections.addItem(constants::AG_TUBE_INTERSECTION, 110, CONNECTOR_INTERSECTION);
		mConnections.addItem(constants::AG_TUBE_RIGHT, 112, CONNECTOR_RIGHT);
		mConnections.addItem(constants::AG_TUBE_LEFT, 111, CONNECTOR_LEFT);

		// Special case code, not thrilled with this
		if (mLandersColonist > 0) { mStructures.addItem(constants::COLONIST_LANDER, 2, SID_COLONIST_LANDER); }
		if (mLandersCargo > 0) { mStructures.addItem(constants::CARGO_LANDER, 1, SID_CARGO_LANDER); }
	}
	else
	{
		mStructures.addItem(constants::LABORATORY, 58, SID_LABORATORY);
		mStructures.addItem(constants::PARK, 75, SID_PARK);
		mStructures.addItem(constants::UNDERGROUND_POLICE, 61, SID_UNDERGROUND_POLICE);
		mStructures.addItem(constants::RECREATION_CENTER, 73, SID_RECREATION_CENTER);
		mStructures.addItem(constants::RESIDENCE, 55, SID_RESIDENCE);
		mStructures.addItem(constants::UNDERGROUND_FACTORY, 69, SID_UNDERGROUND_FACTORY);
		mStructures.addItem(constants::MEDICAL_CENTER, 62, SID_MEDICAL_CENTER);
		mStructures.addItem(constants::NURSERY, 77, SID_NURSERY);
		mStructures.addItem(constants::COMMERCIAL, 66, SID_COMMERCIAL);
		mStructures.addItem(constants::RED_LIGHT_DISTRICT, 76, SID_RED_LIGHT_DISTRICT);
		mStructures.addItem(constants::UNIVERSITY, 63, SID_UNIVERSITY);

		mConnections.addItem(constants::UG_TUBE_INTERSECTION, 113, CONNECTOR_INTERSECTION);
		mConnections.addItem(constants::UG_TUBE_RIGHT, 115, CONNECTOR_RIGHT);
		mConnections.addItem(constants::UG_TUBE_LEFT, 114, CONNECTOR_LEFT);
	}

	updateStructuresAvailability();

	mStructures.sort();
	mConnections.sort();
}


/**
* Updates and draws the UI.
*/
void MapViewState::drawUI()
{
	Renderer& r = Utility<Renderer>::get();

	// Bottom UI
	r.drawBoxFilled(BOTTOM_UI_AREA, 39, 39, 39);
	r.drawBox(BOTTOM_UI_AREA, 21, 21, 21);
	r.drawLine(static_cast<float>(BOTTOM_UI_AREA.x() + 1), static_cast<float>(BOTTOM_UI_AREA.y()), static_cast<float>(BOTTOM_UI_AREA.x() + BOTTOM_UI_AREA.width() - 2), static_cast<float>(BOTTOM_UI_AREA.y()), 56, 56, 56);

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
		resourceShortageMessage(mPlayerResources, StructureTranslator::translateFromString(_item->name));
		mStructures.clearSelection();
		return;
	}

	setStructureID(static_cast<StructureID>(_item->meta), INSERT_STRUCTURE);
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void MapViewState::connectionsSelectionChanged(const IconGrid::IconGridItem* _item)
{
	mRobots.clearSelection();
	mStructures.clearSelection();

	setStructureID(SID_TUBE, INSERT_TUBE);
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

	mInsertMode = INSERT_ROBOT;
	Utility<Renderer>::get().setCursor(POINTER_PLACE_TILE);
}


/**
 * 
 */
void MapViewState::diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, Tile* _t)
{
	// Before doing anything, if we're going down and the depth is not the surface,
	// the assumption is that we've already checked and determined that there's an air shaft
	// so clear it from the tile, disconnect the tile and run a connectedness search.
	if (_t->depth() > 0 && _sel == DiggerDirection::SEL_DOWN)
	{
		Utility<StructureManager>::get().removeStructure(_t->structure());
		Utility<StructureManager>::get().disconnectAll();
		_t->deleteThing();
		_t->connected(false);
		checkConnectedness();
	}

	// Assumes a digger is available.
	Robodigger* r = mRobotPool.getDigger();
	r->startTask(_t->index() + 5); // FIXME: Magic Number
	mRobotPool.insertRobotIntoTable(mRobotList, r, _t);


	if (_sel == DiggerDirection::SEL_DOWN)
	{
		r->direction(DIR_DOWN);
	}
	else if (_sel == DiggerDirection::SEL_NORTH)
	{
		r->direction(DIR_NORTH);
		mTileMap->getTile(_t->x(), _t->y() - 1, _t->depth())->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_SOUTH)
	{
		r->direction(DIR_SOUTH);
		mTileMap->getTile(_t->x(), _t->y() + 1, _t->depth())->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_EAST)
	{
		r->direction(DIR_EAST);
		mTileMap->getTile(_t->x() + 1, _t->y(), _t->depth())->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_WEST)
	{
		r->direction(DIR_WEST);
		mTileMap->getTile(_t->x() - 1, _t->y(), _t->depth())->excavated(true);
	}


	if (!mRobotPool.robotAvailable(ROBOT_DIGGER))
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
	//save(constants::SAVE_GAME_PATH + "test.xml");
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.setMode(FileIo::FILE_SAVE);
	mFileIoDialog.show();
}


/**
 * Click handler for the main menu Load Game button.
 */
void MapViewState::btnLoadGameClicked()
{
	mGameOptionsDialog.hide();
	//load(constants::SAVE_GAME_PATH + "test.xml");
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.setMode(FileIo::FILE_LOAD);
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
	Utility<Renderer>::get().fadeOut(static_cast<float>(constants::FADE_SPEED));
	mQuitCallback();
}


/**
 * Handler for File I/O actions.
 */
void MapViewState::fileIoAction(const std::string& _file, FileIo::FileOperation _op)
{
	if (_op == FileIo::FILE_LOAD)
	{
		try
		{
			load(constants::SAVE_GAME_PATH + _file + ".xml");
		}
		catch (const std::exception& e)
		{
			doNonFatalErrorMessage("Load Failed", e.what());
			return;
		}
	}
	else
	{
		save(constants::SAVE_GAME_PATH + _file + ".xml");
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
 * Player ResourcePool modified, we update the IconGrid
 *
 * \todo	Could be removed and have updateStructureAvailability() used
 *			as the listener instead, but we may want to perform other
 *			functions here so I'm leaving it in - Lee
 */
void MapViewState::playerResourcePoolModified()
{
	updateStructuresAvailability();
}


/**
 * Update IconGridItems availability
 */
void MapViewState::updateStructuresAvailability()
{
	std::string structure;
	for (int sid = 0; sid < SID_COUNT; ++sid)
	{
		structure = StructureTranslator::translateToString(static_cast<StructureID>(sid));
		if (structure.empty())
		{
			continue;
		}

		if (StructureCatalogue::canBuild(mPlayerResources, static_cast<StructureID>(sid)))
		{
			mStructures.itemAvailable(structure, true);
		}
		else
		{
			mStructures.itemAvailable(structure, false);
		}
	}
}
