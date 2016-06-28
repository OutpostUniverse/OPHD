#pragma once

#include "NAS2D/NAS2D.h"

#include "../Common.h"

#include "../Population/PopulationPool.h"

#include "../ResourcePool.h"
#include "../RobotPool.h"

#include "../StructureManager.h"

#include "../Things/Structures/Structure.h"
#include "../Things/Robots/Robots.h"

#include "../Tile.h"
#include "../TileMap.h"

#include "../UI/UI.h"
#include "../UI/DiggerDirection.h"
#include "../UI/FactoryProduction.h"
#include "../UI/IconGrid.h"
#include "../UI/TileInspector.h"

#include "../Constants.h"

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

	GameState(const std::string& map, const std::string& tset);
	~GameState();

protected:

	void initialize();
	State* update();

private:
	typedef map<Robot*, Tile*> RobotMap;
	typedef vector<Pointer> PointerList;

private:
	void onActivate(bool _b);

	void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseUp(MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);

	void drawUI();
	void drawDebug();
	void drawMiniMap();
	void drawResourceInfo();

	void initUi();
	void resetUi();
	void clearSelections();

	void populateStructureMenu();
	void clearMode();

	void placeRobot();
	void placeStructure();
	void placeTubes();

	bool validTubeConnection(int x, int y, StructureID type);
	bool validStructurePlacement(int x, int y);

	void checkRobotSelectionInterface(const std::string rType, int sheetIndex);
	void dozerTaskFinished(Robot* _r);
	void diggerTaskFinished(Robot* _r);
	void minerTaskFinished(Robot* _r);

	bool insertRobot(Robot* robot, Tile* tile);

	void deploySeedLander(int x, int y);
	void insertSeedLander(int x, int y);
	bool landingSiteSuitable(int x, int y);

	void updateMapView();
	void updateRobots();

	void checkConnectedness();

	void changeDepth(int _d);

	void setStructureID(StructureID type, InsertMode mode);

	void save(const std::string& _path);

	// UI Event Handlers
	void btnTurnsClicked();
	void btnTubesPickerClicked();
	void btnRobotPickerClicked();
	void btnStructurePickerClicked();
	void btnToggleConnectednessClicked();

	void structuresSelectionChanged(const std::string& _s);
	void connectionsSelectionChanged(const std::string& _s);
	void robotsSelectionChanged(const std::string& _s);

	void diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, Tile* _t);

	int foodInStorage();
	int foodTotalStorage();

private:
	FpsCounter			mFps;

	Font				mFont;
	Font				mTinyFont;

	TileMap				mTileMap;

	Image				mMapDisplay;
	Image				mHeightMap;
	Image				mUiIcons;

	PointerList			mPointers;
	PointerType			mCurrentPointer;

	Point_2d			mMousePosition;				/**< Current position of the mouse pointer. */
	Point_2d			mTileMapMouseHover;			/**< Tile position the mouse is currently hovering over. */
	Point_2d			mCCLocation;				/**< Location of the Command Center. */

	Rectangle_2d		mMiniMapBoundingBox;
	Rectangle_2d		mResourceInfoBox;

	StructureManager	mStructureManager;			/**< Manager class responsible for managing all structures. */

	ResourcePool		mPlayerResources;			/**< Player's current resources. */

	RobotPool			mRobotPool;					/**< Robots that are currently available for use. */

	RobotMap			mRobotList;					/**< List of active robots and their positions on the map. */

	InsertMode			mInsertMode;				/**< What's being inserted into the TileMap if anything. */
	StructureID			mCurrentStructure;			/**< Structure being placed. */
	RobotType			mCurrentRobot;				/**< Robot being placed. */

	// UI
	Button				mBtnStructures;
	Button				mBtnConnections;
	Button				mBtnRobots;
	Button				mBtnTurns;

	Button				mBtnToggleHeightmap;
	Button				mBtnToggleConnectedness;

	IconGrid			mStructures;
	IconGrid			mRobots;
	IconGrid			mConnections;

	DiggerDirection		mDiggerDirection;
	TileInspector		mTileInspector;
	FactoryProduction	mFactoryProduction;

	// MISCELLANEOUS
	int					mTurnCount;

	bool				mDebug;
	bool				mLeftButtonDown;		/**< Used for mouse drags on the mini map. */

	State*				mReturnState;
};
