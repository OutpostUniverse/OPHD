// ==================================================================================
// = This file implements the UI and UI Event Handlers used by GameState. I separated
// = it into its own file because the GameState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "GameState.h"

#include "UiConstants.h"

using namespace constants;

NAS2D::Rectangle_2d	BOTTOM_UI_AREA;

/**
* Sets up the user interface elements
*/
void GameState::initUi()
{
	EventHandler& e = Utility<EventHandler>::get();
	Renderer& r = Utility<Renderer>::get();

	mDiggerDirection.directionSelected().Connect(this, &GameState::diggerSelectionDialog);
	mDiggerDirection.visible(false);

	mTubesPalette.tubeTypeSelected().Connect(this, &GameState::tubePaletteSelection);
	mTubesPalette.visible(false);

	// Bottom UI
	BOTTOM_UI_AREA(0, r.height() - 185, r.width(), 185);
	mMiniMapBoundingBox(MARGIN, BOTTOM_UI_AREA.y() + MARGIN, mMapDisplay.width(), mMapDisplay.height());
	mResourceInfoBox(mMiniMapBoundingBox.x() + mMiniMapBoundingBox.w() + MARGIN, mMiniMapBoundingBox.y(), 200, mMiniMapBoundingBox.h());

	int posX = r.width() - 34;

	// BUTTONS
	// System
	mBtnSystem.image("ui/icons/system.png");
	mBtnSystem.size(30, 30);
	mBtnSystem.position(posX, BOTTOM_UI_AREA.y() + MARGIN);
	mBtnSystem.click().Connect(this, &GameState::btnSystemClicked);

	mBtnStructurePicker.image("ui/icons/construction.png");
	mBtnStructurePicker.size(30, 30);
	mBtnStructurePicker.type(Button::BUTTON_TOGGLE);
	mBtnStructurePicker.position(posX, BOTTOM_UI_AREA.y() + MARGIN + 32);
	mBtnStructurePicker.click().Connect(this, &GameState::btnStructurePickerClicked);

	mBtnTubePicker.image("ui/icons/tubes.png");
	mBtnTubePicker.size(30, 30);
	mBtnTubePicker.type(Button::BUTTON_TOGGLE);
	mBtnTubePicker.enabled(false);
	mBtnTubePicker.position(posX, BOTTOM_UI_AREA.y() + MARGIN + 64);
	mBtnTubePicker.click().Connect(this, &GameState::btnTubesPickerClicked);

	mBtnRobotPicker.image("ui/icons/robot.png");
	mBtnRobotPicker.size(30, 30);
	mBtnRobotPicker.type(Button::BUTTON_TOGGLE);
	mBtnRobotPicker.enabled(false);
	mBtnRobotPicker.position(posX, BOTTOM_UI_AREA.y() + MARGIN + 96);
	mBtnRobotPicker.click().Connect(this, &GameState::btnRobotPickerClicked);

	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.size(30, 30);
	mBtnTurns.position(posX, BOTTOM_UI_AREA.y() + MARGIN + 128);
	mBtnTurns.click().Connect(this, &GameState::btnTurnsClicked);
	mBtnTurns.enabled(false);


	// Mini Map
	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(20, 20);
	mBtnToggleHeightmap.position(mMiniMapBoundingBox.x(), mMiniMapBoundingBox.y() + mMiniMapBoundingBox.h() + 4);
	mBtnToggleHeightmap.type(Button::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(20, 20);
	mBtnToggleConnectedness.position(mMiniMapBoundingBox.x() + 22, mMiniMapBoundingBox.y() + mMiniMapBoundingBox.h() + 4);
	mBtnToggleConnectedness.type(Button::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().Connect(this, &GameState::btnToggleConnectednessClicked);

	// Menus
	mRobotsMenu.font(mTinyFont);
	mRobotsMenu.width(200);
	mRobotsMenu.position(0, 0);
	mRobotsMenu.visible(false);
	mRobotsMenu.selectionChanged().Connect(this, &GameState::menuRobotsSelectionChanged);

	mStructureMenu.font(mTinyFont);
	mStructureMenu.width(200);
	mStructureMenu.position(0, 0);
	mStructureMenu.visible(false);
	mStructureMenu.selectionChanged().Connect(this, &GameState::menuStructuresSelectionChanged);

	// Initial Structure
	mStructureMenu.addItem(constants::SEED_LANDER);
}


/**
* Hides all non-essential UI elements and untoggles
* all main UI construction buttons.
*/
void GameState::hideUi()
{
	mRobotsMenu.visible(false);
	mStructureMenu.visible(false);
	mDiggerDirection.visible(false);
	mTubesPalette.visible(false);
	mTileInspector.visible(false);
	mDiggerDirection.visible(false);
}


/**
* Adds selection options to the Structure Menu
*/
void GameState::populateStructureMenu()
{
	mStructureMenu.dropAllItems();

	// Above Ground structures only
	if (mTileMap.currentDepth() == 0)
	{
		mStructureMenu.addItem(constants::AGRIDOME);
		mStructureMenu.addItem(constants::CHAP);
	}
	else
	{
	}
}


/**
* Updates and draws the UI.
*/
void GameState::drawUI()
{
	Renderer& r = Utility<Renderer>::get();

	// Bottom UI
	r.drawBoxFilled(BOTTOM_UI_AREA, 200, 200, 200);

	drawMiniMap();
	drawResourceInfo();

	// Buttons
	mBtnToggleHeightmap.update();
	mBtnToggleConnectedness.update();

	mBtnSystem.update();
	mBtnStructurePicker.update();
	mBtnTubePicker.update();
	mBtnRobotPicker.update();

	mBtnTurns.update();
	
	stringstream str;
	str << mTurnCount;
	r.drawText(mTinyFont, str.str(), mBtnTurns.rect().x() + 2, mBtnTurns.rect().y() + mBtnTurns.rect().h() + 2, 0, 0, 0);

	mRobotsMenu.update();
	mStructureMenu.update();

	// UI Containers
	mDiggerDirection.update();
	mTubesPalette.update();
	mTileInspector.update();

	// Always draw last
	mPointers[mCurrentPointer].draw(mMousePosition.x(), mMousePosition.y());
}


/**
 * Handles clicks of the Connectedness Overlay button.
 */
void GameState::btnToggleConnectednessClicked()
{
	mTileMap.toggleShowConnections();
}


/**
 * Handles clicks of the Robot Picker button.
 */
void GameState::btnRobotPickerClicked()
{
	hideUi();

	mBtnStructurePicker.toggle(false);
	mBtnTubePicker.toggle(false);

	if (mBtnRobotPicker.toggled())
		mRobotsMenu.visible(true);
}


/**
 * Handles clicks of the Structure Picker button.
 */
void GameState::btnStructurePickerClicked()
{
	hideUi();

	mBtnRobotPicker.toggle(false);
	mBtnTubePicker.toggle(false);

	if (mBtnStructurePicker.toggled())
		mStructureMenu.visible(true);
}


/**
 * Handles clicks of the Tubes Picker button.
 */
void GameState::btnTubesPickerClicked()
{
	hideUi();

	mBtnRobotPicker.toggle(false);
	mBtnStructurePicker.toggle(false);

	if (mBtnTubePicker.toggled())
		mTubesPalette.visible(true);
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void GameState::menuRobotsSelectionChanged()
{
	mRobotsMenu.visible(false);
	mBtnRobotPicker.toggle(false);

	mInsertMode = INSERT_ROBOT;
	mCurrentPointer = POINTER_PLACE_TILE;
}


/**
* Currently uses a text comparison function. Not inherently bad but
* should really be turned into a key/value pair table for easier lookups.
*/
void GameState::menuStructuresSelectionChanged()
{
	mStructureMenu.visible(false);
	mBtnStructurePicker.toggle(false);

	if (mStructureMenu.selectionText() == constants::SEED_LANDER)
	{
		mCurrentStructure = STRUCTURE_SEED_LANDER;
	}
	else if (mStructureMenu.selectionText() == constants::AGRIDOME)
	{
		mCurrentStructure = STRUCTURE_AGRIDOME;
	}
	else if (mStructureMenu.selectionText() == constants::CHAP)
	{
		mCurrentStructure = STRUCTURE_CHAP;
	}

	mInsertMode = INSERT_STRUCTURE;
	mCurrentPointer = POINTER_PLACE_TILE;
}


void GameState::diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, TilePositionInfo& _tpi)
{
	// Don't dig beyond the dig depth of the planet.
	if (mTileMap.currentDepth() == mTileMap.maxDepth() && _sel == DiggerDirection::SEL_DOWN)
	{
		cout << "GameState::diggerSelectionDialog(): Already at the maximum digging depth." << endl;
		return;
	}

	// Before doing anything, if we're going down and the depth is not the surface,
	// the assumption is that we've already checked and determined that there's an air shaft
	// so clear it from the tile, disconnect the tile and run a connectedness search.
	if (_tpi.depth > 0 && _sel == DiggerDirection::SEL_DOWN)
	{
		mStructureManager.removeStructure(_tpi.tile->structure());
		mStructureManager.disconnectAll();
		_tpi.tile->deleteThing();
		_tpi.tile->connected(false);
		checkConnectedness();
	}

	// Assumes a digger is available.
	Robodigger* r = mRobotPool.getDigger();
	r->startTask(_tpi.tile->index() + 5);
	insertRobot(r, _tpi.tile, _tpi.x, _tpi.y, _tpi.depth);


	if (_sel == DiggerDirection::SEL_DOWN)
	{
		r->direction(DIR_DOWN);
	}
	else if (_sel == DiggerDirection::SEL_NORTH)
	{
		r->direction(DIR_NORTH);
		mTileMap.getTile(_tpi.x, _tpi.y - 1, _tpi.depth)->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_SOUTH)
	{
		r->direction(DIR_SOUTH);
		mTileMap.getTile(_tpi.x, _tpi.y + 1, _tpi.depth)->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_EAST)
	{
		r->direction(DIR_EAST);
		mTileMap.getTile(_tpi.x + 1, _tpi.y, _tpi.depth)->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_WEST)
	{
		r->direction(DIR_WEST);
		mTileMap.getTile(_tpi.x - 1, _tpi.y, _tpi.depth)->excavated(true);
	}


	if (!mRobotPool.robotAvailable(RobotPool::ROBO_DIGGER))
	{
		mRobotsMenu.removeItem(constants::ROBODIGGER);
		clearMode();
	}

	mDiggerDirection.visible(false);
}


/**
 * Handler for the Tubes Pallette dialog.
 */
void GameState::tubePaletteSelection(ConnectorDir _t, bool _b)
{
	mBtnStructurePicker.toggle(false);
	mBtnTubePicker.toggle(false);
	mBtnRobotPicker.toggle(false);

	if (_b)
	{
		mCurrentStructure = STRUCTURE_NONE;
		clearMode();
	}

	if (_t == CONNECTOR_INTERSECTION)
		mCurrentStructure = STRUCTURE_TUBE_INTERSECTION;
	else if (_t == CONNECTOR_RIGHT)
		mCurrentStructure = STRUCTURE_TUBE_RIGHT;
	else if (_t == CONNECTOR_LEFT)
		mCurrentStructure = STRUCTURE_TUBE_LEFT;
	else
	{
		mCurrentStructure = STRUCTURE_NONE;
		return;
	}

	mInsertMode = INSERT_TUBE;
	mCurrentPointer = POINTER_PLACE_TILE;
}


/**
* Turns button clicked.
*/
void GameState::btnTurnsClicked()
{
	clearMode();

	mStructureManager.update();
	checkConnectedness();
	mStructureManager.processResources(mPlayerResources);

	updateRobots();

	Structure* cc = mTileMap.getTile(mCCLocation.x(), mCCLocation.y(), TileMap::LEVEL_SURFACE)->structure();
	if (cc->state() == Structure::OPERATIONAL)
	{
		populateStructureMenu();
		mBtnStructurePicker.enabled(true);
	}
	else
		mBtnStructurePicker.enabled(false);

	mTurnCount++;
}


/**
* System button clicked.
*/
void GameState::btnSystemClicked()
{
	NAS2D::postQuitEvent();
}