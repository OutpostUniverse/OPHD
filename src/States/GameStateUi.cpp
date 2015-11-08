// ==================================================================================
// = This file implements the UI and UI Event Handlers used by GameState. I separated
// = it into its own file because the GameState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "GameState.h"

#include "../Constants.h"

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
	mDiggerDirection.position(r.height() - constants::BOTTOM_UI_HEIGHT + constants::MARGIN, mMiniMapBoundingBox.w() + constants::MARGIN * 3 + constants::MINI_MAP_BUTTON_SIZE);

	mTubesPalette.tubeTypeSelected().Connect(this, &GameState::tubePaletteSelection);
	mTubesPalette.visible(false);
	mTubesPalette.position(r.height() - constants::BOTTOM_UI_HEIGHT + constants::MARGIN, mMiniMapBoundingBox.w() + constants::MARGIN * 3 + constants::MINI_MAP_BUTTON_SIZE);

	// Bottom UI
	BOTTOM_UI_AREA(0, r.height() - constants::BOTTOM_UI_HEIGHT, r.width(), constants::BOTTOM_UI_HEIGHT);

	mResourceInfoBox(mMiniMapBoundingBox.x() + mMiniMapBoundingBox.w() + constants::MARGIN * 2 + constants::MINI_MAP_BUTTON_SIZE, mMiniMapBoundingBox.y(), constants::RESOURCE_BOX_WIDTH, mMiniMapBoundingBox.h());

	// X Position of the main UI buttons
	int posX = r.width() - 34;

	// BUTTONS
	// System
	mBtnStructures.image("ui/icons/construction.png");
	mBtnStructures.size(constants::MAIN_BUTTON_SIZE);
	mBtnStructures.type(Button::BUTTON_TOGGLE);
	mBtnStructures.position(constants::MARGIN, BOTTOM_UI_AREA.y() + MARGIN);
	mBtnStructures.click().Connect(this, &GameState::btnStructurePickerClicked);

	mBtnConnections.image("ui/icons/tubes.png");
	mBtnConnections.size(constants::MAIN_BUTTON_SIZE);
	mBtnConnections.type(Button::BUTTON_TOGGLE);
	mBtnConnections.enabled(false);
	mBtnConnections.position(constants::MARGIN, BOTTOM_UI_AREA.y() + MARGIN + (constants::MAIN_BUTTON_SIZE + constants::MARGIN));
	mBtnConnections.click().Connect(this, &GameState::btnTubesPickerClicked);

	mBtnRobots.image("ui/icons/robot.png");
	mBtnRobots.size(constants::MAIN_BUTTON_SIZE);
	mBtnRobots.type(Button::BUTTON_TOGGLE);
	mBtnRobots.enabled(false);
	mBtnRobots.position(constants::MARGIN, BOTTOM_UI_AREA.y() + MARGIN + (constants::MAIN_BUTTON_SIZE + constants::MARGIN) * 2);
	mBtnRobots.click().Connect(this, &GameState::btnRobotPickerClicked);

	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.size(constants::MAIN_BUTTON_SIZE);
	mBtnTurns.position(constants::MARGIN, r.height() - constants::MARGIN - MAIN_BUTTON_SIZE);
	mBtnTurns.click().Connect(this, &GameState::btnTurnsClicked);
	mBtnTurns.enabled(false);


	// Mini Map
	mMiniMapBoundingBox(r.width() - mMapDisplay.width() - constants::MARGIN, BOTTOM_UI_AREA.y() + constants::MARGIN, mMapDisplay.width(), mMapDisplay.height());

	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(constants::MINI_MAP_BUTTON_SIZE);
	mBtnToggleHeightmap.position(mMiniMapBoundingBox.x() - constants::MINI_MAP_BUTTON_SIZE - constants::MARGIN_TIGHT, mMiniMapBoundingBox.y());
	mBtnToggleHeightmap.type(Button::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(constants::MINI_MAP_BUTTON_SIZE);
	mBtnToggleConnectedness.position(mMiniMapBoundingBox.x() - constants::MINI_MAP_BUTTON_SIZE - constants::MARGIN_TIGHT, mMiniMapBoundingBox.y() + constants::MINI_MAP_BUTTON_SIZE + constants::MARGIN_TIGHT);
	mBtnToggleConnectedness.type(Button::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().Connect(this, &GameState::btnToggleConnectednessClicked);

	// Menus
	mRobotsMenu.font(mTinyFont);
	mRobotsMenu.width(200);
	mRobotsMenu.position(constants::MARGIN * 2 + constants::MAIN_BUTTON_SIZE, BOTTOM_UI_AREA.y() + MARGIN);
	mRobotsMenu.hide();
	mRobotsMenu.selectionChanged().Connect(this, &GameState::menuRobotsSelectionChanged);

	mStructureMenu.font(mTinyFont);
	mStructureMenu.width(200);
	mStructureMenu.position(constants::MARGIN * 2 + constants::MAIN_BUTTON_SIZE, BOTTOM_UI_AREA.y() + MARGIN);
	mStructureMenu.hide();
	mStructureMenu.selectionChanged().Connect(this, &GameState::menuStructuresSelectionChanged);

	mFactoryProduction.hide();

	// Initial Structure
	mStructureMenu.addItem(constants::SEED_LANDER);
}


/**
* Hides all non-essential UI elements and untoggles
* all main UI construction buttons.
*/
void GameState::hideUi()
{
	mRobotsMenu.hide();
	mStructureMenu.hide();
	mDiggerDirection.hide();
	mTubesPalette.hide();
	mTileInspector.hide();
	mDiggerDirection.hide();
	mFactoryProduction.hide();
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
	r.drawBoxFilled(BOTTOM_UI_AREA, 240, 240, 240);
	r.drawBox(BOTTOM_UI_AREA, 25, 130, 215);

	drawMiniMap();
	drawResourceInfo();

	// Buttons
	mBtnToggleHeightmap.update();
	mBtnToggleConnectedness.update();

	mBtnStructures.update();
	mBtnConnections.update();
	mBtnRobots.update();

	mBtnTurns.update();
	
	mRobotsMenu.update();
	mStructureMenu.update();

	// UI Containers
	mDiggerDirection.update();
	mTubesPalette.update();
	mTileInspector.update();
	mFactoryProduction.update();

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

	mBtnStructures.toggle(false);
	mBtnConnections.toggle(false);

	if (mBtnRobots.toggled())
		mRobotsMenu.visible(true);
}


/**
 * Handles clicks of the Structure Picker button.
 */
void GameState::btnStructurePickerClicked()
{
	hideUi();

	mBtnRobots.toggle(false);
	mBtnConnections.toggle(false);

	if (mBtnStructures.toggled())
		mStructureMenu.visible(true);
}


/**
 * Handles clicks of the Tubes Picker button.
 */
void GameState::btnTubesPickerClicked()
{
	hideUi();

	mBtnRobots.toggle(false);
	mBtnStructures.toggle(false);

	if (mBtnConnections.toggled())
		mTubesPalette.visible(true);
}


/**
 * Handles clicks of the Robot Selection Menu.
 */
void GameState::menuRobotsSelectionChanged()
{
	mRobotsMenu.visible(false);
	mBtnRobots.toggle(false);

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
	mBtnStructures.toggle(false);

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
	mBtnStructures.toggle(false);
	mBtnConnections.toggle(false);
	mBtnRobots.toggle(false);

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
		mBtnStructures.enabled(true);
	}
	else
		mBtnStructures.enabled(false);

	mTurnCount++;
}
