// ==================================================================================
// = This file implements the UI and UI Event Handlers used by GameState. I separated
// = it into its own file because the GameState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "GameState.h"
#include "GameStateHelper.h"

#include "../Constants.h"

#include "../RobotTranslator.h"
#include "../StructureTranslator.h"

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
	mDiggerDirection.hide();
	mDiggerDirection.position(r.screenCenterX() - mDiggerDirection.width() / 2, r.height() / 2 - 125);

	mTileInspector.position(r.screenCenterX() - mTileInspector.width() / 2, r.height() / 2 - 175);
	mTileInspector.hide();

	mStructureInspector.position(r.screenCenterX() - mStructureInspector.width() / 2, r.height() / 2 - 175);
	mStructureInspector.hide();

	mFactoryProduction.position(r.screenCenterX() - mFactoryProduction.width() / 2, 175);
	mFactoryProduction.hide();


	mWindowStack.addWindow(&mTileInspector);
	mWindowStack.addWindow(&mStructureInspector);
	mWindowStack.addWindow(&mFactoryProduction);


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
	mStructures.font(mTinyFont);
	mStructures.sheetPath("ui/structures.png");
	mStructures.position(constants::MARGIN * 2 + constants::MAIN_BUTTON_SIZE, BOTTOM_UI_AREA.y() + MARGIN);
	mStructures.size(r.width() - constants::MARGIN * 4 - constants::MARGIN_TIGHT - constants::MAIN_BUTTON_SIZE - constants::MINI_MAP_BUTTON_SIZE - mMiniMapBoundingBox.w(), BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mStructures.iconSize(46);
	mStructures.iconMargin(constants::MARGIN_TIGHT);
	mStructures.showTooltip(true);
	mStructures.hide();
	mStructures.selectionChanged().Connect(this, &GameState::structuresSelectionChanged);

	mConnections.font(mTinyFont);
	mConnections.sheetPath("ui/structures.png");
	mConnections.position(constants::MARGIN * 2 + constants::MAIN_BUTTON_SIZE, BOTTOM_UI_AREA.y() + MARGIN);
	mConnections.size(r.width() - constants::MARGIN * 4 - constants::MARGIN_TIGHT - constants::MAIN_BUTTON_SIZE - constants::MINI_MAP_BUTTON_SIZE - mMiniMapBoundingBox.w(), BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mConnections.iconSize(46);
	mConnections.iconMargin(constants::MARGIN_TIGHT);
	mConnections.hide();
	mConnections.selectionChanged().Connect(this, &GameState::connectionsSelectionChanged);
	mConnections.sorted(false);

	mRobots.font(mTinyFont);
	mRobots.sheetPath("ui/robots.png");
	mRobots.position(constants::MARGIN * 2 + constants::MAIN_BUTTON_SIZE, BOTTOM_UI_AREA.y() + MARGIN);
	mRobots.size(r.width() - constants::MARGIN * 4 - constants::MARGIN_TIGHT - constants::MAIN_BUTTON_SIZE - constants::MINI_MAP_BUTTON_SIZE - mMiniMapBoundingBox.w(), BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mRobots.iconSize(46);
	mRobots.iconMargin(constants::MARGIN_TIGHT);
	mRobots.showTooltip(true);
	mRobots.hide();
	mRobots.selectionChanged().Connect(this, &GameState::robotsSelectionChanged);

	// Initial Structures
	mStructures.addItem(constants::SEED_LANDER, 0);

	mConnections.addItem(constants::AG_TUBE_INTERSECTION, 110);
	mConnections.addItem(constants::AG_TUBE_LEFT, 111);
	mConnections.addItem(constants::AG_TUBE_RIGHT, 112);
}


/**
 * Hides all non-essential UI elements.
 */
void GameState::resetUi()
{
	clearMode();

	mStructures.hide();
	mConnections.hide();
	mRobots.hide();

	clearSelections();

	mDiggerDirection.hide();
	mFactoryProduction.hide();
	mStructureInspector.hide();
	mTileInspector.hide();

	mBtnStructures.toggle(false);
	mBtnConnections.toggle(false);
	mBtnRobots.toggle(false);
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
	if (mTileMap->currentDepth() == 0)
	{
		mStructures.addItem(constants::AGRIDOME, 5);
		mStructures.addItem(constants::CHAP, 3);
		mStructures.addItem(constants::FUSION_REACTOR, 21);
		mStructures.addItem(constants::SURFACE_POLICE, 23);
		mStructures.addItem(constants::SMELTER, 4);
		mStructures.addItem(constants::SOLAR_PLANT, 10);
		mStructures.addItem(constants::STORAGE_TANKS, 8);
		mStructures.addItem(constants::SURFACE_FACTORY, 11);

		mConnections.addItem(constants::AG_TUBE_INTERSECTION, 110);
		mConnections.addItem(constants::AG_TUBE_RIGHT, 112);
		mConnections.addItem(constants::AG_TUBE_LEFT, 111);
	}
	else
	{
		mStructures.addItem(constants::PARK, 75);
		mStructures.addItem(constants::UNDERGROUND_POLICE, 61);
		mStructures.addItem(constants::RECREATION_CENTER, 73);
		mStructures.addItem(constants::RESIDENCE, 55);
		mStructures.addItem(constants::UNDERGROUND_FACTORY, 69);

		mConnections.addItem(constants::UG_TUBE_INTERSECTION, 113);
		mConnections.addItem(constants::UG_TUBE_RIGHT, 115);
		mConnections.addItem(constants::UG_TUBE_LEFT, 114);
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
	
	// Menus
	mRobots.update();
	mStructures.update();
	mConnections.update();

	// UI Containers
	mDiggerDirection.update();

	mWindowStack.update();

	// Always draw last
	mPointers[mCurrentPointer].draw(mMousePosition.x(), mMousePosition.y());
}


/**
 * Handles clicks of the Connectedness Overlay button.
 */
void GameState::btnToggleConnectednessClicked()
{
	mTileMap->toggleShowConnections();
}


/**
 * Handles clicks of the Structure Picker button.
 */
void GameState::btnStructurePickerClicked()
{
	bool toggled = mBtnStructures.toggled();
	resetUi();

	mBtnStructures.toggle(toggled);
	mBtnRobots.toggle(false);
	mBtnConnections.toggle(false);

	if (toggled)
		mStructures.show();
}


/**
 * Handles clicks of the Tubes Picker button.
 */
void GameState::btnTubesPickerClicked()
{
	bool toggled = mBtnConnections.toggled();
	resetUi();

	mBtnConnections.toggle(toggled);
	mBtnRobots.toggle(false);
	mBtnStructures.toggle(false);

	if (toggled)
		mConnections.visible(true);
}





/**
* Handles clicks of the Robot Picker button.
*/
void GameState::btnRobotPickerClicked()
{
	bool toggled = mBtnRobots.toggled();
	resetUi();

	mBtnRobots.toggle(toggled);
	mBtnStructures.toggle(false);
	mBtnConnections.toggle(false);

	if (toggled)
		mRobots.visible(true);
}


/**
* Currently uses a text comparison function. Not inherently bad but
* should really be turned into a key/value pair table for easier lookups.
*/
void GameState::structuresSelectionChanged(const std::string& _s)
{
	setStructureID(StructureTranslator::translateFromString(_s), INSERT_STRUCTURE);
}


/**
* Handler for the Tubes Pallette dialog.
*/
void GameState::connectionsSelectionChanged(const std::string& _s)
{
	setStructureID(SID_TUBE, INSERT_TUBE);
}


/**
* Handles clicks of the Robot Selection Menu.
*/
void GameState::robotsSelectionChanged(const std::string& _s)
{
	mCurrentRobot = RobotTranslator::translateFromString(_s);

	if(mCurrentRobot == ROBOT_NONE)
	{
		clearMode();
		return;
	}

	mInsertMode = INSERT_ROBOT;
	mCurrentPointer = POINTER_PLACE_TILE;
}


void GameState::diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, Tile* _t)
{
	// Don't dig beyond the dig depth of the planet.
	if (mTileMap->currentDepth() == mTileMap->maxDepth() && _sel == DiggerDirection::SEL_DOWN)
	{
		mAiVoiceNotifier.notify(AiVoiceNotifier::MAX_DIGGING_DEPTH_REACHED);
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
	insertRobotIntoTable(mRobotList, r, _t);


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
* Turns button clicked.
*/
void GameState::btnTurnsClicked()
{
	clearMode();

	mStructureManager.disconnectAll();
	checkConnectedness();
	mStructureManager.update(mPlayerResources);

	// Update storage capacity
	mPlayerResources.capacity(totalStorage(mStructureManager.structureList(Structure::STRUCTURE_STORAGE)));
	
	ResourcePool truck;
	truck.capacity(100);

	auto mines = mStructureManager.structureList(Structure::STRUCTURE_MINE);
	auto smelters = mStructureManager.structureList(Structure::STRUCTURE_SMELTER);

	// Move ore from mines to smelters
	for (size_t m = 0; m < mines.size(); ++m)
	{
		if (mines[m]->disabled() || mines[m]->destroyed())
			continue; // consider a different control path.

		truck.commonMetalsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, 25));
		truck.commonMineralsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, 25));
		truck.rareMetalsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_RARE_METALS_ORE, 25));
		truck.rareMineralsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, 25));

		for (size_t s = 0; s < smelters.size(); ++s)
		{
			if(smelters[s]->operational())
				smelters[s]->production().pushResources(truck);
		}

		if (!truck.empty())
			mines[m]->storage().pushResources(truck);
	}

	// Move refined resources from smelters to storage tanks
	for (size_t s = 0; s < smelters.size(); ++s)
	{
		if (smelters[s]->disabled() || smelters[s]->destroyed())
			continue; // consider a different control path.

		truck.commonMetals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_COMMON_METALS, 25));
		truck.commonMinerals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_COMMON_MINERALS, 25));
		truck.rareMetals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_RARE_METALS, 25));
		truck.rareMinerals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_RARE_MINERALS, 25));

		mPlayerResources.pushResources(truck);

		if (!truck.empty())
		{
			smelters[s]->storage().pushResources(truck);
			break;	// we're at max capacity in our storage, dump what's left in the smelter it came from and barf.
		}
	}

	updateRobots();

	Structure* cc = mTileMap->getTile(mCCLocation.x(), mCCLocation.y(), TileMap::LEVEL_SURFACE)->structure();
	if (cc->state() == Structure::OPERATIONAL)
	{
		populateStructureMenu();
		mBtnStructures.enabled(true);
	}
	else
		mBtnStructures.enabled(false);

	mTurnCount++;
}
