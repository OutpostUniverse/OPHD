#ifndef __START_STATE__
#define __START_STATE__

#include "NAS2D/NAS2D.h"

#include "Population/PopulationPool.h"

#include "Resources.h"
#include "RobotPool.h"

#include "StructureManager.h"

#include "Things/Structures/Structure.h"
#include "Things/Robots/Robots.h"

#include "TilePositionInfo.h"
#include "TileMap.h"

#include "UI/UI.h"
#include "UI/DiggerDirection.h"
#include "UI/TileInspector.h"
#include "UI/TubesPalette.h"

#include "Strings.h"

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


enum StructureType
{
	STRUCTURE_NONE,
	STRUCTURE_AGRIDOME,
	STRUCTURE_AIR_SHAFT,
	STRUCTURE_CHAP,
	STRUCTURE_COMMAND_CENTER,
	STRUCTURE_SEED_FACTORY,
	STRUCTURE_SEED_LANDER,
	STRUCTURE_SEED_POWER,
	STRUCTURE_SEED_SMELTER,
	STRUCTURE_TUBE_INTERSECTION,
	STRUCTURE_TUBE_RIGHT,
	STRUCTURE_TUBE_LEFT
};


enum InsertMode
{
	INSERT_NONE,
	INSERT_ROBOT,
	INSERT_TUBE,
	INSERT_STRUCTURE
};


/**
 * Why do I need this? This is honestly a "WTF was I thinking?"
 * moment. This could be done much more simply.
 */
struct DiggerTile
{
	DiggerTile(): tile(0), depth(0), x(0), y(0)
	{}

	void clear()
	{
		tile = 0;
		depth = 0;
		x = 0;
		y = 0;
	}

	void operator()(Tile* _t, int _d, int _x, int _y)
	{
		tile = _t;
		depth = _d;
		x = _x;
		y = _y;
	}

	Tile* tile;
	int depth, x, y;
};


class GameState: public State
{
public:

	GameState(const std::string& map);
	~GameState();

protected:

	void initialize();
	State* update();

private:

	typedef map<Thing*, TilePositionInfo> ThingMap;
	typedef map<Robot*, TilePositionInfo> RobotMap;

	typedef vector<Pointer> PointerList;

	void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseUp(MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int rX, int rY);
	void onMouseWheel(int x, int y);

	void drawUI();
	void drawDebug();
	void drawMiniMap();
	void drawResourceInfo();

	void initUi();
	void hideUi();

	void populateStructureMenu();

	void placeRobot();
	void placeStructure();
	void placeTubes();

	bool validTubeConnection(Tile *tile, Direction dir);
	bool validStructurePlacement(Tile *tile, Direction dir);

	void dozerTaskFinished(Robot* _r);
	void diggerTaskFinished(Robot* _r);
	void minerTaskFinished(Robot* _r);

	bool insertThing(Thing* thing, Tile* tile, int x, int y, int depth);
	bool insertRobot(Robot* robot, Tile* tile, int x, int y, int depth);

	void deploySeedLander(int x, int y);
	bool landingSiteSuitable(int x, int y);

	void updateMapView();

	void updateRobots();

	void clearMode();

	void checkConnectedness();

	// UI Even Handlers
	void btnSystemClicked();
	void btnTurnsClicked();
	void btnTubesPickerClicked();
	void btnRobotPickerClicked();
	void btnStructurePickerClicked();

	void btnToggleConnectednessClicked();

	void diggerSelectionDialog(DiggerDirection::DiggerSelection sel);

	void tubePaletteSelection(ConnectorDir _cd);

	void menuRobotsSelectionChanged();
	void menuStructuresSelectionChanged();

	FpsCounter			mFps;
	Timer				mMoveTimer;

	Font				mFont;
	Font				mTinyFont;

	TileMap				mTileMap;

	DiggerTile			mDiggerTile;

	Image				mBackground;
	Image				mMapDisplay;
	Image				mHeightMap;

	PointerList			mPointers;
	PointerType			mCurrentPointer;

	Point_2d			mMousePosition;

	Point_2d			mCCLocation;				/**< Location of the Command Center. */

	Rectangle_2d		mMiniMapBoundingBox;
	Rectangle_2d		mResourceInfoBox;

	Resources			mPlayerResources;
	PopulationPool		mPopulationPool;			/**< Population available to the Player. */

	RobotPool			mRobotPool;					/**< Robots that are currently available for use. */

	ThingMap			mThingList;					/**< List of Thing objects currently in game play */
	RobotMap			mRobotList;					/**< List of active robots and their positions on the map. */

	StructureManager	mStructureManager;			/**< Manager class responsible for managing all structures. */

	StructureType		mCurrentStructure;			/**< Structure being placed. */

	// UI
	Button				mBtnSystem;
	Button				mBtnStructurePicker;
	Button				mBtnTubePicker;
	Button				mBtnRobotPicker;
	Button				mBtnTurns;

	Button				mBtnToggleHeightmap;
	Button				mBtnToggleConnectedness;

	Menu				mRobotsMenu;
	Menu				mStructureMenu;

	DiggerDirection		mDiggerDirection;

	TubesPalette		mTubesPalette;

	TileInspector		mTileInspector;

	InsertMode			mInsertMode;

	int					mTurnCount;

	bool				mDebug;
	bool				mLeftButtonDown;		/**< Used for mouse drags on the mini map. */

	State*				mReturnState;
};



#endif