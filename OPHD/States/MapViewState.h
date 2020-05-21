#pragma once

#include "NAS2D/Signal.h"

#include "MapViewStateHelper.h"
#include "MainReportsUiState.h"
#include "Wrapper.h"

#include "../Common.h"
#include "../Constants.h"

#include "../Map/Tile.h"
#include "../Map/TileMap.h"

#include "../PopulationPool.h"
#include "../Population/Population.h"

#include "../ResourcePool.h"
#include "../RobotPool.h"

#include "../Things/Structures/Structure.h"
#include "../Things/Robots/Robots.h"

#include "../UI/Gui.h"

#include <string>


enum PointerType
{
	POINTER_NORMAL,
	POINTER_PLACE_TILE,
	POINTER_INSPECT,
	NUMBER_OF_POINTER_TYPES = PointerType::POINTER_INSPECT
};


enum InsertMode
{
	INSERT_NONE,
	INSERT_ROBOT,
	INSERT_TUBE,
	INSERT_STRUCTURE
};


class MapViewState : public Wrapper
{
public:
	enum PopulationLevel
	{
		POPULATION_SMALL = 1,
		POPULATION_LARGE = 2
	};

public:
	using QuitCallback = NAS2D::Signals::Signal<>;
	using ReportsUiCallback = NAS2D::Signals::Signal<>;
	using MapChangedCallback = NAS2D::Signals::Signal<>;

public:
	MapViewState(const std::string& savegame);
	MapViewState(const std::string& siteMap, const std::string& tileSet, int depth, int mineCount, constants::PlanetHostility hostility);
	~MapViewState() override;

	void setPopulationLevel(PopulationLevel popLevel);

	ReportsUiCallback& showReporstUi() { return mReportsUiCallback; }
	QuitCallback& quit() { return mQuitCallback; }
	MapChangedCallback& mapChanged() { return mMapChangedCallback; }

	void focusOnStructure(Structure* s);

protected:
	void initialize() override;
	State* update() override;

private:
	void _deactivate() override;
	void _activate() override;
	
	// EVENT HANDLERS
	void onActivate(bool newActiveValue);
	void onKeyDown(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseDoubleClick(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);
	void onMouseWheel(int x, int y);
	void onWindowResized(int w, int h);

	// ROBOT EVENT HANDLERS
	void dozerTaskFinished(Robot* robot);
	void diggerTaskFinished(Robot* robot);
	void minerTaskFinished(Robot* robot);

	// DRAWING FUNCTIONS
	void drawUI();
	void drawMiniMap();
	void drawNavInfo();
	bool drawNavIcon(NAS2D::Renderer& renderer, const NAS2D::Rectangle<int>& currentIconBounds, const NAS2D::Rectangle<int>& subImageBounds, const NAS2D::Color& iconColor, const NAS2D::Color& iconHighlightColor);

	void drawResourceInfo();
	void drawRobotInfo();

	// INSERT OBJECT HANDLING
	void deployCargoLander();
	void deployColonistLander();
	void deploySeedLander(int x, int y);
	void insertSeedLander(int x, int y);
	void insertTube(ConnectorDir dir, int depth, Tile* tile);

	void placeRobot();
	void placeStructure();
	void placeTubes();
	
	NAS2D::Rectangle<int> tubeStart;
	void placeTubeStart();
	void placeTubeEnd();

	void setStructureID(StructureID type, InsertMode mode);

	// MISCELLANEOUS UTILITY FUNCTIONS
	int foodInStorage();
	int foodTotalStorage();
	void setMinimapView();

	void checkConnectedness();
	bool changeDepth(int newDepth);

	void pullRobotFromFactory(ProductType pt, Factory& factory);
	void factoryProductionComplete(Factory& factory);

	void mineFacilityExtended(MineFacility* mf);

	// TURN LOGIC
	void checkColonyShip();
	void nextTurn();
	void updatePopulation();
	void updateCommercial();
	void updateMorale();
	void updateResidentialCapacity();
	void updateResources();
	void updateRobots();


	// SAVE GAME MANAGEMENT FUNCTIONS
	void readRobots(NAS2D::Xml::XmlElement* element);
	void readStructures(NAS2D::Xml::XmlElement* element);
	void readTurns(NAS2D::Xml::XmlElement* element);
	void readPopulation(NAS2D::Xml::XmlElement* element);

	void scrubRobotList();

	void load(const std::string& filePath);
	void save(const std::string& filePath);

	// UI MANAGEMENT FUNCTIONS
	void clearMode();
	void clearSelections();

	void hideUi();
	void unhideUi();
	void initUi();
	void resetUi();

	bool modalUiElementDisplayed() const;

	void setupUiPositions(NAS2D::Vector<int> size);

	void checkRobotSelectionInterface(const std::string& rType, int sheetIndex, RobotType);

	void populateStructureMenu();

	void updateCurrentLevelString(int currentDepth);
	void updateStructuresAvailability();

	// UI EVENT HANDLERS
	void btnTurnsClicked();
	void btnToggleConnectednessClicked();

	void btnSaveGameClicked();
	void btnLoadGameClicked();
	void btnReturnToGameClicked();
	void btnGameOverClicked();
	void playerResourcePoolModified();

	void structuresSelectionChanged(const IconGrid::IconGridItem*);
	void connectionsSelectionChanged(const IconGrid::IconGridItem*);
	void robotsSelectionChanged(const IconGrid::IconGridItem*);

	void diggerSelectionDialog(DiggerDirection::DiggerSelection selection, Tile* tile);

	void fileIoAction(const std::string& filePath, FileIo::FileOperation fileOp);


private:
	TileMap* mTileMap = nullptr;

	NAS2D::Image mBackground; /**< Background image drawn behind the tile map. */
	NAS2D::Image mMapDisplay; /**< Satellite view of the Site Map. */
	NAS2D::Image mHeightMap; /**< Height view of the Site Map. */
	NAS2D::Image mUiIcons; /**< User interface icons. */

	NAS2D::Point<int> mTileMapMouseHover; /**< Tile position the mouse is currently hovering over. */

	NAS2D::Rectangle<int> mMiniMapBoundingBox; /**< Area of the site map display. */

	// POOL'S
	ResourcePool mPlayerResources; /**< Player's current resources. */
	RobotPool mRobotPool; /**< Robots that are currently available for use. */
	PopulationPool mPopulationPool;

	RobotTileTable mRobotList; /**< List of active robots and their positions on the map. */

	InsertMode mInsertMode = InsertMode::INSERT_NONE; /**< What's being inserted into the TileMap if anything. */
	StructureID mCurrentStructure = StructureID::SID_NONE; /**< Structure being placed. */
	RobotType mCurrentRobot = RobotType::ROBOT_NONE; /**< Robot being placed. */

	Population mPopulation;

	//Music mBgMusic;

	// USER INTERFACE
	Button mBtnTurns;
	Button mBtnToggleHeightmap;
	Button mBtnToggleConnectedness;

	IconGrid mStructures;
	IconGrid mRobots;
	IconGrid mConnections;

	DiggerDirection mDiggerDirection;
	FactoryProduction mFactoryProduction;
	FileIo mFileIoDialog;
	GameOverDialog mGameOverDialog;
	GameOptionsDialog mGameOptionsDialog;
	MajorEventAnnouncement mAnnouncement;
	MineOperationsWindow mMineOperationsWindow;
	PopulationPanel mPopulationPanel;
	ResourceBreakdownPanel mResourceBreakdownPanel;
	StructureInspector mStructureInspector;
	TileInspector mTileInspector;
	WarehouseInspector mWarehouseInspector;

	WindowStack mWindowStack;

	// SIGNALS
	QuitCallback mQuitCallback;
	ReportsUiCallback mReportsUiCallback;
	MapChangedCallback mMapChangedCallback;

	// MISCELLANEOUS
	int mTurnCount = 0;

	int mCurrentMorale = constants::DEFAULT_STARTING_MORALE;
	int mPreviousMorale = constants::DEFAULT_STARTING_MORALE;

	int mLandersColonist = 0;
	int mLandersCargo = 0;

	int mResidentialCapacity = 0;

	bool mLeftButtonDown = false;
	bool mLoadingExisting = false;
	bool mPinResourcePanel = false;
	bool mPinPopulationPanel = false;

	std::string mExistingToLoad; /**< Filename of the existing game to load. */
};
