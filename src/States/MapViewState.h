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

using namespace NAS2D;

/**
 * Needs to match insertion order in MapViewState::initialize()
 */
enum PointerType
{
	POINTER_NORMAL,
	POINTER_PLACE_TILE,
	POINTER_INSPECT,
	NUMBER_OF_POINTER_TYPES = POINTER_INSPECT
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
	using QuitCallback = NAS2D::Signals::Signal0<void>;
	using ReportsUiCallback = NAS2D::Signals::Signal0<void>;
	using MapChangedCallback = NAS2D::Signals::Signal0<void>;

public:
	MapViewState(const std::string& savegame);
	MapViewState(const std::string& siteMap, const std::string& tileSet, int depth, int mineCount, constants::PlanetHostility hostility);
	virtual ~MapViewState();

	void setPopulationLevel(PopulationLevel popLevel);

	ReportsUiCallback& showReporstUi() { return mReportsUiCallback; }
	QuitCallback& quit() { return mQuitCallback; }
	MapChangedCallback& mapChanged() { return mMapChangedCallback; }

	void focusOnStructure(Structure* s);

protected:
	void initialize();
	State* update();

private:
	virtual void _deactivate() final;
	virtual void _activate() final;
	
	// EVENT HANDLERS
	void onActivate(bool newActiveValue);
	void onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat);
	void onMouseDown(EventHandler::MouseButton button, int x, int y);
	void onMouseDoubleClick(EventHandler::MouseButton button, int x, int y);
	void onMouseUp(EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);
	void onMouseWheel(int x, int y);
	void onWindowResized(int w, int h);

	// ROBOT EVENT HANDLERS
	void dozerTaskFinished(Robot* r);
	void diggerTaskFinished(Robot* r);
	void minerTaskFinished(Robot* r);

	// DRAWING FUNCTIONS
	void drawUI();
	void drawMiniMap();
	void drawNavInfo();
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
	void readRobots(Xml::XmlElement* element);
	void readStructures(Xml::XmlElement* element);
	void readTurns(Xml::XmlElement* element);
	void readPopulation(Xml::XmlElement* element);

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

	void setupUiPositions(int, int);

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
	FpsCounter			mFps;							/**< Main FPS Counter. */

	TileMap*			mTileMap = nullptr;				/**<  */

	Image				mBackground;					/**< Background image drawn behind the tile map. */
	Image				mMapDisplay;					/**< Satellite view of the Site Map. */
	Image				mHeightMap;						/**< Height view of the Site Map. */
	Image				mUiIcons;						/**< User interface icons. */

	Point_2d			mTileMapMouseHover;				/**< Tile position the mouse is currently hovering over. */

	Rectangle_2d		mMiniMapBoundingBox;			/**< Area of the site map display. */

	// POOL'S
	ResourcePool		mPlayerResources;				/**< Player's current resources. */
	RobotPool			mRobotPool;						/**< Robots that are currently available for use. */
	PopulationPool		mPopulationPool;				/**<  */

	RobotTileTable		mRobotList;						/**< List of active robots and their positions on the map. */

	InsertMode			mInsertMode = INSERT_NONE;		/**< What's being inserted into the TileMap if anything. */
	StructureID			mCurrentStructure = SID_NONE;	/**< Structure being placed. */
	RobotType			mCurrentRobot = ROBOT_NONE;		/**< Robot being placed. */

	Population			mPopulation;					/**<  */

	//Music				mBgMusic;						/**<  */

	// USER INTERFACE
	Button				mBtnTurns;						/**< Turns Button. */
	Button				mBtnToggleHeightmap;			/**< Height Map Toggle Button. */
	Button				mBtnToggleConnectedness;		/**< Connectedness view toggle button. */

	IconGrid			mStructures;					/**< Structures pick view. */
	IconGrid			mRobots;						/**< Robots pick view. */
	IconGrid			mConnections;					/**< Tubes pick view. */

	DiggerDirection		mDiggerDirection;				/**< Digger direction window. */
	FactoryProduction	mFactoryProduction;				/**< Factory Production window. */
	FileIo				mFileIoDialog;					/**< File IO window. */
	GameOverDialog		mGameOverDialog;				/**< Game over window. */
	GameOptionsDialog	mGameOptionsDialog;				/**< Options List window. */
	MajorEventAnnouncement	mAnnouncement;				/**< Announcements window. */
	MineOperationsWindow	mMineOperationsWindow;		/**< Mine Operations window. */
	PopulationPanel		mPopulationPanel;				/**< Population panel. */
	ResourceBreakdownPanel	mResourceBreakdownPanel;	/**< Resource Breakdown Panel. */
	StructureInspector	mStructureInspector;			/**< Structure Inspector window. */
	TileInspector		mTileInspector;					/**< Tile Inspector window. */
	WarehouseInspector	mWarehouseInspector;			/**< Warehouse Inspector window. */

	WindowStack			mWindowStack;					/**< Window stack manager. */

	// SIGNALS
	QuitCallback		mQuitCallback;					/**< Signal for posting quit event. */
	ReportsUiCallback	mReportsUiCallback;				/**< Signal for bringing the Main Reports UI up. */
	MapChangedCallback	mMapChangedCallback;			/**< Signal indicating that the map changed. */

	// MISCELLANEOUS
	int					mTurnCount = 0;					/**<  */

	int					mCurrentMorale = constants::DEFAULT_STARTING_MORALE;
	int					mPreviousMorale = constants::DEFAULT_STARTING_MORALE;

	int					mLandersColonist = 0;			/**<  */
	int					mLandersCargo = 0;				/**<  */

	int					mResidentialCapacity = 0;		/**<  */

	bool				mLeftButtonDown = false;		/**< Used for mouse drags on the mini map. */
	bool				mLoadingExisting = false;		/**< Flag used for loading an existing game. */
	bool				mPinResourcePanel = false;		/**<  */
	bool				mPinPopulationPanel = false;	/**<  */

	std::string			mExistingToLoad;				/**< Filename of the existing game to load. */
};
