#pragma once

#include "NAS2D/NAS2D.h"

#include "GameStateHelper.h"

#include "../AiVoiceNotifier.h"

#include "../Common.h"
#include "../Constants.h"

#include "../Map/Tile.h"
#include "../Map/TileMap.h"

#include "../PopulationPool.h"
#include "../Population/Population.h"

#include "../ResourcePool.h"
#include "../RobotPool.h"

#include "../StructureManager.h"

#include "../Things/Structures/Structure.h"
#include "../Things/Robots/Robots.h"

#include "../UI/Gui.h"

using namespace NAS2D;

/**
 * Needs to match insertion order in GameState::initialize()
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


class GameState: public State
{
public:
	enum PopulationLevel
	{
		POPULATION_SMALL = 1,
		POPULATION_LARGE = 2
	};

public:
	GameState(const std::string& savegame);
	GameState(const std::string& map, const std::string& tset, int _d, int _minecount);
	~GameState();

	void setPopulationLevel(PopulationLevel _level);

protected:
	void initialize();
	State* update();

private:
	// EVENT HANDLERS
	void onActivate(bool _b);
	void onKeyDown(EventHandler::KeyCode key, EventHandler::KeyModifier mod, bool repeat);
	void onMouseDown(EventHandler::MouseButton button, int x, int y);
	void onMouseUp(EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);
	void onMouseWheel(int x, int y);
	void onWindowResized(int w, int h);

	// ROBOT EVENT HANDLERS
	void dozerTaskFinished(Robot* _r);
	void diggerTaskFinished(Robot* _r);
	void minerTaskFinished(Robot* _r);

	// DRAWING FUNCTIONS
	void drawUI();
	void drawDebug();
	void drawMiniMap();
	void drawNavInfo();
	void drawResourceInfo();
	void drawRobotInfo();

	// INSERT OBJECT HANDLING
	void deployCargoLander();
	void deployColonistLander();
	void deploySeedLander(int x, int y);
	void insertSeedLander(int x, int y);
	void insertTube(ConnectorDir _dir, int depth, Tile* t);

	void placeRobot();
	void placeStructure();
	void placeTubes();

	void setStructureID(StructureID type, InsertMode mode);

	// MISCELLANEOUS UTILITY FUNCTIONS
	int foodInStorage();
	int foodTotalStorage();
	void setMinimapView();

	void checkConnectedness();
	bool changeDepth(int _d);

	void GameState::pullRobotFromFactory(ProductType pt, Factory& factory);
	void factoryProductionComplete(Factory& factory);

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
	void readRobots(Xml::XmlElement* _ti);
	void readStructures(Xml::XmlElement* _ti);
	void readTurns(Xml::XmlElement* _ti);
	void readPopulation(Xml::XmlElement* _ti);

	void scrubRobotList();

	void load(const std::string& _path);
	void save(const std::string& _path);

	// UI MANAGEMENT FUNCTIONS
	void clearMode();
	void clearSelections();

	void hideUi();
	void initUi();
	void resetUi();

	void setupUiPositions();

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

	void diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, Tile* _t);

	void fileIoAction(const std::string& _file, FileIo::FileOperation _op);

private:
	FpsCounter			mFps;							/**< Main FPS Counter. */

	Font				mFont;							/**< Large Font. */
	Font				mTinyFont;						/**< Small Font. */
	Font				mTinyFontBold;					/**< Small Bold Font. */

	TileMap*			mTileMap = nullptr;				/**<  */

	Image				mBackground;					/**< Background image drawn behind the tile map. */
	Image				mMapDisplay;					/**< Satellite view of the Site Map. */
	Image				mHeightMap;						/**< Height view of the Site Map. */
	Image				mUiIcons;						/**< User interface icons. */

	Point_2d			mMousePosition;					/**< Current position of the mouse pointer. */
	Point_2d			mTileMapMouseHover;				/**< Tile position the mouse is currently hovering over. */
	Point_2d			mCCLocation;					/**< Location of the Command Center. */

	Rectangle_2d		mMiniMapBoundingBox;			/**< Area of the site map display. */

	StructureManager	mStructureManager;				/**< Manager class responsible for managing all structures. */

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
	PopulationPanel		mPopulationPanel;				/**< Population panel. */
	StructureInspector	mStructureInspector;			/**< Structure Inspector window. */
	TileInspector		mTileInspector;					/**< Tile Inspector window. */
	WarehouseInspector	mWarehouseInspector;			/**< Warehouse Inspector window. */

	WindowStack			mWindowStack;					/**< Window stack manager. */

	// MISCELLANEOUS
	int					mTurnCount = 0;					/**<  */

	int					mCurrentMorale = constants::DEFAULT_STARTING_MORALE;
	int					mPreviousMorale = constants::DEFAULT_STARTING_MORALE;

	int					mLandersColonist = 0;			/**<  */
	int					mLandersCargo = 0;				/**<  */

	int					mResidentialCapacity = 0;		/**<  */

	bool				mDebug = false;					/**< Display debug information. */
	bool				mLeftButtonDown = false;		/**< Used for mouse drags on the mini map. */
	bool				mLoadingExisting = false;		/**< Flag used for loading an existing game. */

	std::string			mExistingToLoad;				/**< Filename of the existing game to load. */

	State*				mReturnState = this;			/**<  */
};
