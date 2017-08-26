// ==================================================================================
// = This file implements the UI and UI Event Handlers used by GameState. I separated
// = it into its own file because the GameState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "GameState.h"
#include "GameStateHelper.h"

#include "PlanetSelectState.h"

#include "../Constants.h"

#include "../RobotTranslator.h"
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


/**
 * Performs common computations for window centering and casts the resulting
 * fractional value to an int.
 *
 * \note	Truncating the fractional value is intentional.
 */
int centerWindowWidth(float width)
{
	return static_cast<int>(Utility<Renderer>::get().center_x() - width / 2);
}


/**
 * Performs common computations for window centering and casts the resulting
 * fractional value to an int.
 *
 * \note	Truncating the fractional value is intentional.
 */
int centerWindowHeight(float height)
{
	return static_cast<int>(Utility<Renderer>::get().center_y() - height / 2);
}



/**
 * Sets up the user interface elements
 * 
 * \note	The explicit casts to int to truncate floating point values to force
 *			window positions to a whole number.
 */
void GameState::initUi()
{
	Renderer& r = Utility<Renderer>::get();

	mDiggerDirection.directionSelected().connect(this, &GameState::diggerSelectionDialog);
	mDiggerDirection.hide();

	mTileInspector.position(static_cast<int>(r.center_x() - mTileInspector.width() / 2), static_cast<int>(r.height() / 2 - 175));
	mTileInspector.hide();

	mStructureInspector.position(static_cast<int>(r.center_x() - mStructureInspector.width() / 2), static_cast<int>(r.height() / 2 - 175));
	mStructureInspector.hide();

	mFactoryProduction.position(static_cast<int>(r.center_x() - mFactoryProduction.width() / 2), 175);
	mFactoryProduction.hide();

	mFileIoDialog.setMode(FileIo::FILE_SAVE);
	mFileIoDialog.fileOperation().connect(this, &GameState::fileIoAction);
	mFileIoDialog.anchored(true);
	mFileIoDialog.hide();

	mPopulationPanel.position(580, constants::RESOURCE_ICON_SIZE + 4 + constants::MARGIN_TIGHT);
	mPopulationPanel.font(mTinyFont);
	mPopulationPanel.population(&mPopulation);
	mPopulationPanel.morale(&mCurrentMorale);
	mPopulationPanel.old_morale(&mPreviousMorale);

	mGameOverDialog.returnToMainMenu().connect(this, &GameState::btnGameOverClicked);
	mGameOverDialog.hide();

	mGameOptionsDialog.SaveGame().connect(this, &GameState::btnSaveGameClicked);
	mGameOptionsDialog.LoadGame().connect(this, &GameState::btnLoadGameClicked);
	mGameOptionsDialog.returnToGame().connect(this, &GameState::btnReturnToGameClicked);
	mGameOptionsDialog.returnToMainMenu().connect(this, &GameState::btnGameOverClicked);
	mGameOptionsDialog.hide();

	mAnnouncement.hide();

	mWindowStack.addWindow(&mTileInspector);
	mWindowStack.addWindow(&mStructureInspector);
	mWindowStack.addWindow(&mFactoryProduction);
	mWindowStack.addWindow(&mDiggerDirection);
	mWindowStack.addWindow(&mAnnouncement);

	BOTTOM_UI_AREA(0, static_cast<int>(r.height() - constants::BOTTOM_UI_HEIGHT), static_cast<int>(r.width()), constants::BOTTOM_UI_HEIGHT);

	// BUTTONS
	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.position(static_cast<float>(mMiniMapBoundingBox.x() - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT), static_cast<float>(r.height() - constants::MARGIN - MAIN_BUTTON_SIZE));
	mBtnTurns.size(static_cast<float>(constants::MAIN_BUTTON_SIZE));
	mBtnTurns.click().connect(this, &GameState::btnTurnsClicked);
	mBtnTurns.enabled(false);

	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(static_cast<float>(constants::MAIN_BUTTON_SIZE));
	mBtnToggleHeightmap.type(Button::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(static_cast<float>(constants::MAIN_BUTTON_SIZE));
	mBtnToggleConnectedness.type(Button::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().connect(this, &GameState::btnToggleConnectednessClicked);

	// Menus
	mRobots.font(mTinyFont);
	mRobots.sheetPath("ui/robots.png");
	mRobots.position(static_cast<float>(mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mRobots.size(52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mRobots.iconSize(46);
	mRobots.iconMargin(constants::MARGIN_TIGHT);
	mRobots.showTooltip(true);
	mRobots.selectionChanged().connect(this, &GameState::robotsSelectionChanged);

	mConnections.font(mTinyFont);
	mConnections.sheetPath("ui/structures.png");
	mConnections.position(static_cast<float>(mRobots.positionX() - constants::MARGIN_TIGHT - 52), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mConnections.size(52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mConnections.iconSize(46);
	mConnections.iconMargin(constants::MARGIN_TIGHT);
	mConnections.selectionChanged().connect(this, &GameState::connectionsSelectionChanged);
	mConnections.sorted(false);

	mStructures.font(mTinyFont);
	mStructures.sheetPath("ui/structures.png");
	mStructures.position(static_cast<float>(constants::MARGIN), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mStructures.size(mConnections.positionX() -  constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mStructures.iconSize(46);
	mStructures.iconMargin(constants::MARGIN_TIGHT);
	mStructures.showTooltip(true);
	mStructures.selectionChanged().connect(this, &GameState::structuresSelectionChanged);

	mPlayerResources.resourceObserver().connect(this, &GameState::playerResourcePoolModified);

	// Initial Structures
	mStructures.addItem(constants::SEED_LANDER, 0);
}


void GameState::setupUiPositions()
{
	Renderer& r = Utility<Renderer>::get();

	// Bottom UI Area
	BOTTOM_UI_AREA(0, static_cast<int>(r.height() - constants::BOTTOM_UI_HEIGHT), static_cast<int>(r.width()), constants::BOTTOM_UI_HEIGHT);

	// Menu / System Icon
	MENU_ICON(r.width() - constants::MARGIN_TIGHT * 2 - constants::RESOURCE_ICON_SIZE, 0, constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2, constants::RESOURCE_ICON_SIZE + constants::MARGIN_TIGHT * 2);

	// NAVIGATION BUTTONS
	// Bottom line
	MOVE_DOWN_ICON(r.width() - constants::MARGIN - 32, r.height() - constants::BOTTOM_UI_HEIGHT - 65, 32, 32);
	MOVE_EAST_ICON(MOVE_DOWN_ICON.x() - (32 + constants::MARGIN_TIGHT), MOVE_DOWN_ICON.y() + 8, 32, 16);
	MOVE_SOUTH_ICON(MOVE_DOWN_ICON.x() - 2 * (32 + constants::MARGIN_TIGHT), MOVE_DOWN_ICON.y() + 8, 32, 16);

	// Top line
	MOVE_UP_ICON(MOVE_DOWN_ICON.x(), MOVE_DOWN_ICON.y() - constants::MARGIN_TIGHT - 32, 32, 32);
	MOVE_NORTH_ICON(MOVE_UP_ICON.x() - (32 + constants::MARGIN_TIGHT), MOVE_UP_ICON.y() + 8, 32, 16);
	MOVE_WEST_ICON(MOVE_UP_ICON.x() - 2 * (32 + constants::MARGIN_TIGHT), MOVE_UP_ICON.y() + 8, 32, 16);

	// Mini Map
	mMiniMapBoundingBox(static_cast<int>(r.width() - mMapDisplay.width() - constants::MARGIN), static_cast<int>(BOTTOM_UI_AREA.y() + constants::MARGIN), mMapDisplay.width(), mMapDisplay.height());

	// Position UI Buttons
	mBtnTurns.position(static_cast<float>(mMiniMapBoundingBox.x() - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT), static_cast<float>(r.height() - constants::MARGIN - MAIN_BUTTON_SIZE));
	mBtnToggleHeightmap.position(mBtnTurns.positionX(), static_cast<float>(mMiniMapBoundingBox.y()));
	mBtnToggleConnectedness.position(mBtnTurns.positionX(), static_cast<float>(mMiniMapBoundingBox.y() + constants::MAIN_BUTTON_SIZE + constants::MARGIN_TIGHT));

	// UI Panels
	mRobots.position(static_cast<float>(mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mConnections.position(static_cast<float>(mRobots.positionX() - constants::MARGIN_TIGHT - 52), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));
	mStructures.position(static_cast<float>(constants::MARGIN), static_cast<float>(BOTTOM_UI_AREA.y() + MARGIN));

	mStructures.size(mConnections.positionX() - constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mStructures.iconMargin(constants::MARGIN_TIGHT);

	// Anchored window positions
	mFileIoDialog.position(centerWindowWidth(mFileIoDialog.width()), 50);
	mGameOverDialog.position(centerWindowWidth(mGameOverDialog.width()), centerWindowHeight(mGameOverDialog.height()) - 100);
	mAnnouncement.position(centerWindowWidth(mAnnouncement.width()), centerWindowHeight(mAnnouncement.height()) - 100);
	mGameOptionsDialog.position(centerWindowWidth(mGameOptionsDialog.width()), centerWindowHeight(mGameOptionsDialog.height()) - 100);

	mDiggerDirection.position(centerWindowWidth(mDiggerDirection.width()), static_cast<int>(r.height() / 2) - 125);
}


/**
 * Hides ALL UI elements.
 */
void GameState::hideUi()
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
 * Hides all non-essential UI elements.
 */
void GameState::resetUi()
{
	clearMode();

	clearSelections();

	mDiggerDirection.hide();
	mFactoryProduction.hide();
	mStructureInspector.hide();
	mTileInspector.hide();
}


void GameState::clearSelections()
{
	mStructures.clearSelection();
	mConnections.clearSelection();
	mRobots.clearSelection();
}


/**
* Adds selection options to the Structure Menu
*/
void GameState::populateStructureMenu()
{
	mStructures.dropAllItems();
	mConnections.dropAllItems();

	// Above Ground structures only
	if (mStructureManager.count() == 0)
	{
		if(mTileMap->currentDepth() == constants::DEPTH_SURFACE)
			mStructures.addItem(constants::SEED_LANDER, 0);
	}
	else if (mTileMap->currentDepth() == constants::DEPTH_SURFACE)
	{
		mStructures.addItem(constants::AGRIDOME, 5);
		mStructures.addItem(constants::CHAP, 3);
		mStructures.addItem(constants::FUSION_REACTOR, 21);
		mStructures.addItem(constants::HOT_LABORATORY, 18);
		mStructures.addItem(constants::ROBOT_COMMAND, 14);
		mStructures.addItem(constants::COMM_TOWER, 22);
		mStructures.addItem(constants::SURFACE_POLICE, 23);
		mStructures.addItem(constants::SMELTER, 4);
		mStructures.addItem(constants::SOLAR_PLANT, 10);
		mStructures.addItem(constants::STORAGE_TANKS, 8);
		mStructures.addItem(constants::SURFACE_FACTORY, 11);
		mStructures.addItem(constants::WAREHOUSE, 9);

		mConnections.addItem(constants::AG_TUBE_INTERSECTION, 110);
		mConnections.addItem(constants::AG_TUBE_RIGHT, 112);
		mConnections.addItem(constants::AG_TUBE_LEFT, 111);


		// Special case code, not thrilled with this
		if (mLandersColonist > 0) { mStructures.addItem(constants::COLONIST_LANDER, 2); }
		if (mLandersCargo > 0) { mStructures.addItem(constants::CARGO_LANDER, 1); }
	}
	else
	{
		mStructures.addItem(constants::LABORATORY, 58);
		mStructures.addItem(constants::PARK, 75);
		mStructures.addItem(constants::UNDERGROUND_POLICE, 61);
		mStructures.addItem(constants::RECREATION_CENTER, 73);
		mStructures.addItem(constants::RESIDENCE, 55);
		mStructures.addItem(constants::UNDERGROUND_FACTORY, 69);
		mStructures.addItem(constants::MEDICAL_CENTER, 62);
		mStructures.addItem(constants::NURSERY, 77);
		mStructures.addItem(constants::COMMERCIAL, 66);
		mStructures.addItem(constants::RED_LIGHT_DISTRICT, 76);
		mStructures.addItem(constants::UNIVERSITY, 63);

		mConnections.addItem(constants::UG_TUBE_INTERSECTION, 113);
		mConnections.addItem(constants::UG_TUBE_RIGHT, 115);
		mConnections.addItem(constants::UG_TUBE_LEFT, 114);
	}
	updateStructuresAvailability();
}


/**
* Updates and draws the UI.
*/
void GameState::drawUI()
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
void GameState::btnToggleConnectednessClicked()
{
	mTileMap->toggleShowConnections();
}


/**
* Currently uses a text comparison function. Not inherently bad but
* should really be turned into a key/value pair table for easier lookups.
*/
void GameState::structuresSelectionChanged(const std::string& _s)
{
	mConnections.clearSelection();
	mRobots.clearSelection();

	// Check availability
	if (!mStructures.itemAvailable(_s))
	{
		Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::INSUFFICIENT_RESOURCES);
		cout << "GameState::placeStructure(): Insufficient resources to build structure." << endl;
		mStructures.clearSelection();
		return;
	}

	setStructureID(StructureTranslator::translateFromString(_s), INSERT_STRUCTURE);
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void GameState::connectionsSelectionChanged(const std::string& _s)
{
	mRobots.clearSelection();
	mStructures.clearSelection();

	setStructureID(SID_TUBE, INSERT_TUBE);
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void GameState::robotsSelectionChanged(const std::string& _s)
{
	mConnections.clearSelection();
	mStructures.clearSelection();

	mCurrentRobot = RobotTranslator::translateFromString(_s);

	if(mCurrentRobot == ROBOT_NONE)
	{
		clearMode();
		return;
	}

	mInsertMode = INSERT_ROBOT;
	Utility<Renderer>::get().setCursor(POINTER_PLACE_TILE);
}


void GameState::diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, Tile* _t)
{
	// Don't dig beyond the dig depth of the planet.
	if (mTileMap->currentDepth() == mTileMap->maxDepth() && _sel == DiggerDirection::SEL_DOWN)
	{
		Utility<AiVoiceNotifier>::get().notify(AiVoiceNotifier::MAX_DIGGING_DEPTH_REACHED);
		cout << "GameState::diggerSelectionDialog(): Already at the maximum digging depth." << endl;
		return;
	}

	// Before doing anything, if we're going down and the depth is not the surface,
	// the assumption is that we've already checked and determined that there's an air shaft
	// so clear it from the tile, disconnect the tile and run a connectedness search.
	if (_t->depth() > 0 && _sel == DiggerDirection::SEL_DOWN)
	{
		mStructureManager.removeStructure(_t->structure());
		mStructureManager.disconnectAll();
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

void GameState::btnSaveGameClicked()
{
	mGameOptionsDialog.hide();
	//save(constants::SAVE_GAME_PATH + "test.xml");
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.setMode(FileIo::FILE_SAVE);
	mFileIoDialog.show();
}

void GameState::btnLoadGameClicked()
{
	mGameOptionsDialog.hide();
	//load(constants::SAVE_GAME_PATH + "test.xml");
	mFileIoDialog.scanDirectory(constants::SAVE_GAME_PATH);
	mFileIoDialog.setMode(FileIo::FILE_LOAD);
	mFileIoDialog.show();

}

void GameState::btnReturnToGameClicked()
{
	mGameOptionsDialog.hide();
}

void GameState::btnGameOverClicked()
{
	mReturnState = new PlanetSelectState();
	Utility<Renderer>::get().fadeOut(static_cast<float>(constants::FADE_SPEED));
}

void GameState::fileIoAction(const std::string& _file, FileIo::FileOperation _op)
{
	_op == FileIo::FILE_LOAD ? load(constants::SAVE_GAME_PATH + _file + ".xml") : save(constants::SAVE_GAME_PATH + _file + ".xml");

	mFileIoDialog.hide();
}


/**
 * Turns button clicked.
 */
void GameState::btnTurnsClicked()
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
void GameState::playerResourcePoolModified()
{
	updateStructuresAvailability();
}


/**
 * Update IconGridItems availability
 */
void GameState::updateStructuresAvailability()
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
