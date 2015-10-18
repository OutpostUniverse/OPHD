#ifndef __START_STATE__
#define __START_STATE__

#include "NAS2D/NAS2D.h"

#include "../Population/PopulationPool.h"

#include "../Resources.h"
#include "../RobotPool.h"

#include "../StructureManager.h"

#include "../Things/Structures/Structure.h"
#include "../Things/Robots/Robots.h"

#include "../TilePositionInfo.h"
#include "../TileMap.h"

#include "../UI/UI.h"
#include "../UI/DiggerDirection.h"
#include "../UI/TileInspector.h"
#include "../UI/TubesPalette.h"

#include "../Strings.h"

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



class GameState: public State
{
public:

	GameState(const std::string& map, const std::string& tset);
	~GameState();

protected:

	void initialize();
	State* update();

private:

	typedef map<Robot*, TilePositionInfo> RobotMap;

	typedef vector<Pointer> PointerList;

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
	void hideUi();

	void populateStructureMenu();
	void clearMode();

	void placeRobot();
	void placeStructure();
	void placeTubes();

	bool validTubeConnection(Tile *tile, Direction dir);
	bool validStructurePlacement(Tile *tile, Direction dir);

	void dozerTaskFinished(Robot* _r);
	void diggerTaskFinished(Robot* _r);
	void minerTaskFinished(Robot* _r);

	bool insertRobot(Robot* robot, Tile* tile, int x, int y, int depth);

	void deploySeedLander(int x, int y);
	bool landingSiteSuitable(int x, int y);

	void updateMapView();
	void updateRobots();

	void checkConnectedness();

	void changeDepth(int _d);


	// UI Even Handlers
	void btnSystemClicked();
	void btnTurnsClicked();
	void btnTubesPickerClicked();
	void btnRobotPickerClicked();
	void btnStructurePickerClicked();
	void btnToggleConnectednessClicked();

	void menuRobotsSelectionChanged();
	void menuStructuresSelectionChanged();

	void diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, TilePositionInfo& _tpi);
	void tubePaletteSelection(ConnectorDir _cd, bool _b);

	FpsCounter			mFps;
	Timer				mMoveTimer;

	Font				mFont;
	Font				mTinyFont;

	TileMap				mTileMap;

	Image				mBackground;
	Image				mMapDisplay;
	Image				mHeightMap;
	Image				mUiIcons;

	PointerList			mPointers;
	PointerType			mCurrentPointer;

	Point_2d			mMousePosition;
	Point_2d			mCCLocation;				/**< Location of the Command Center. */

	Rectangle_2d		mMiniMapBoundingBox;
	Rectangle_2d		mResourceInfoBox;

	StructureManager	mStructureManager;			/**< Manager class responsible for managing all structures. */

	Resources			mPlayerResources;			/**< Player's current resources. */
	PopulationPool		mPopulationPool;			/**< Population available to the Player. */
	RobotPool			mRobotPool;					/**< Robots that are currently available for use. */

	RobotMap			mRobotList;					/**< List of active robots and their positions on the map. */

	InsertMode			mInsertMode;				/**< What's being inserted into the TileMap if anything. */
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

	// MISCELLANEOUS
	int					mTurnCount;

	bool				mDebug;
	bool				mLeftButtonDown;		/**< Used for mouse drags on the mini map. */

	State*				mReturnState;
};


#endif