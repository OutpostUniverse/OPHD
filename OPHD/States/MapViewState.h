#pragma once

#include "Wrapper.h"
#include "CrimeRateUpdate.h"
#include "CrimeExecution.h"

#include "Planet.h"

#include "../Constants/Numbers.h"
#include "../Constants/UiConstants.h"

#include "../Common.h"
#include "../StorableResources.h"
#include "../RobotPool.h"
#include "../PopulationPool.h"
#include "../Population/Population.h"
#include "../Things/Structures/Structure.h"

#include "../UI/NotificationArea.h"
#include "../UI/NotificationWindow.h"
#include "../UI/DiggerDirection.h"
#include "../UI/FactoryProduction.h"
#include "../UI/FileIo.h"
#include "../UI/GameOverDialog.h"
#include "../UI/GameOptionsDialog.h"
#include "../UI/IconGrid.h"
#include "../UI/MajorEventAnnouncement.h"
#include "../UI/MineOperationsWindow.h"
#include "../UI/PopulationPanel.h"
#include "../UI/ResourceBreakdownPanel.h"
#include "../UI/RobotInspector.h"
#include "../UI/StructureInspector.h"
#include "../UI/TileInspector.h"
#include "../UI/WarehouseInspector.h"
#include "../UI/ResourceInfoBar.h"
#include "../UI/RobotDeploymentSummary.h"
#include "../UI/MiniMap.h"
#include "../UI/DetailMap.h"
#include "../UI/NavControl.h"

#include "../UI/Core/WindowStack.h"
#include "../UI/Core/ToolTip.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Rectangle.h>

#include <string>
#include <memory>
#include <map>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

namespace micropather
{
	class MicroPather;
}

struct MapCoordinate;
class Tile;
class TileMap;
class MapView;
class MainReportsUiState;


enum PointerType
{
	POINTER_NORMAL,
	POINTER_PLACE_TILE
};


enum class InsertMode
{
	None,
	Robot,
	Tube,
	Structure
};


class MapViewState : public Wrapper
{
public:
	enum class PopulationLevel
	{
		Small = 1,
		Large = 2
	};

public:
	using QuitSignal = NAS2D::Signal<>;
	using ReportsUiSignal = NAS2D::Signal<>;
	using MapChangedSignal = NAS2D::Signal<>;

public:
	MapViewState(MainReportsUiState& mainReportsState, const std::string& savegame);
	MapViewState(MainReportsUiState& mainReportsState, const Planet::Attributes& planetAttributes, Difficulty selectedDifficulty);
	~MapViewState() override;

	void setPopulationLevel(PopulationLevel popLevel);

	ReportsUiSignal::Source& showReporstUi() { return mReportsUiSignal; }
	QuitSignal::Source& quit() { return mQuitSignal; }
	MapChangedSignal::Source& mapChanged() { return mMapChangedSignal; }

	void focusOnStructure(Structure* s);

	Difficulty difficulty() { return mDifficulty; }
	void difficulty(Difficulty difficulty);

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
	void onWindowResized(NAS2D::Vector<int> newSize);

	void onInspect(const MapCoordinate& tilePosition, bool inspectModifier);
	void onInspectStructure(Structure& structure, bool inspectModifier);
	void onInspectRobot(Robot& robot);
	void onInspectTile(Tile& tile);

	void onClickMap();

	void onSystemMenu();

	// ROBOT EVENT HANDLERS
	void onDozerTaskComplete(Robot* robot);
	void onDiggerTaskComplete(Robot* robot);
	void onMinerTaskComplete(Robot* robot);

	// DRAWING FUNCTIONS
	void drawUI();
	void drawSystemButton() const;

	// INSERT OBJECT HANDLING
	void onDeployCargoLander();
	void onDeployColonistLander();
	void onDeploySeedLander(NAS2D::Point<int> point);
	void insertSeedLander(NAS2D::Point<int> point);
	void insertTube(ConnectorDir dir, int depth, Tile* tile);

	void placeTubes(Tile* tile);
	void placeStructure(Tile* tile);
	void placeRobot(Tile* tile);

	void placeRobodozer(Tile&);
	void placeRobodigger(Tile&);
	void placeRobominer(Tile&);

	void setStructureID(StructureID type, InsertMode mode);

	// MISCELLANEOUS UTILITY FUNCTIONS
	void updateFood();
	void transferFoodToCommandCenter();

	void checkCommRangeOverlay();
	void checkSurfacePoliceOverlay();
	void resetPoliceOverlays();
	void fillRangedAreaList(std::vector<Tile*>& tileList, Tile& centerTile, int range);
	void fillRangedAreaList(std::vector<Tile*>& tileList, Tile& centerTile, int range, int depth);
	void checkConnectedness();
	void changeViewDepth(int);

	void pullRobotFromFactory(ProductType pt, Factory& factory);
	void onFactoryProductionComplete(Factory& factory);

	void onMineFacilityExtend(MineFacility* mf);

	void updatePlayerResources();

	// TURN LOGIC
	void checkColonyShip();
	void nextTurn();
	void updatePopulation();
	void updateCommercial();
	void updateMaintenance();
	void updateMorale();
	void updateResidentialCapacity();
	void updateBiowasteRecycling();
	void updateResources();
	void updateRoads();
	void updateRobots();

	void findMineRoutes();
	void transportOreFromMines();
	void transportResourcesToStorage();

	void checkAgingStructures();
	void checkNewlyBuiltStructures();

	// SAVE GAME MANAGEMENT FUNCTIONS
	std::map<int, Robot*> readRobots(NAS2D::Xml::XmlElement* element);
	void readStructures(NAS2D::Xml::XmlElement* element, const std::map<int, Robot*>& idToRobotMap);
	void readTurns(NAS2D::Xml::XmlElement* element);
	void readPopulation(NAS2D::Xml::XmlElement* element);
	void readMoraleChanges(NAS2D::Xml::XmlElement*);

	void scrubRobotList();

	void load(const std::string& filePath);
	void save(const std::string& filePath);
	NAS2D::Xml::XmlElement* serializeProperties();

	// UI MANAGEMENT FUNCTIONS
	void clearMode();
	void clearSelections();

	void hideUi();
	void unhideUi();
	void initUi();
	void resetUi();

	bool modalUiElementDisplayed() const;

	void setupUiPositions(NAS2D::Vector<int> size);

	void populateRobotMenu();
	void populateStructureMenu();

	void updateStructuresAvailability();

	// UI EVENT HANDLERS
	void onTurns();
	void setOverlay(std::vector<Tile*>& tileList, Tile::Overlay overlay);
	void clearOverlays();
	void clearOverlay(std::vector<Tile*>& tileList);
	void updateOverlays();
	void changePoliceOverlayDepth(int oldDepth, int newDepth);
	void onToggleHeightmap();
	void onToggleConnectedness();
	void onToggleCommRangeOverlay();
	void onToggleRouteOverlay();
	void onTogglePoliceOverlay();

	void onNotificationClicked(const NotificationArea::Notification&);

	void onSaveGame();
	void onLoadGame();
	void onReturnToGame();
	void onGameOver();

	void onStructuresSelectionChange(const IconGrid::IconGridItem*);
	void onConnectionsSelectionChange(const IconGrid::IconGridItem*);
	void onRobotsSelectionChange(const IconGrid::IconGridItem*);

	void onDiggerSelectionDialog(Direction direction, Tile* tile);

	void onFileIoAction(const std::string& filePath, FileIo::FileOperation fileOp);

	void onNotificationWindowTakeMeThere(const MapCoordinate& position);

private:
	MainReportsUiState& mMainReportsState;
	TileMap* mTileMap{nullptr};
	std::unique_ptr<MapView> mMapView;
	CrimeRateUpdate mCrimeRateUpdate;
	CrimeExecution mCrimeExecution;

	Planet::Attributes mPlanetAttributes;
	Difficulty mDifficulty = Difficulty::Medium;

	const NAS2D::Image mUiIcons{"ui/icons.png"}; /**< User interface icons. */
	const NAS2D::Image mBackground{"sys/bg1.png"}; /**< Background image drawn behind the tile map. */

	MapCoordinate mMouseTilePosition;

	NAS2D::Rectangle<int> mMiniMapBoundingBox; /**< Area of the site map display. */

	int mFood{0};

	// POOLS
	StorableResources mResourcesCount;
	RobotPool mRobotPool; /**< Robots that are currently available for use. */
	PopulationPool mPopulationPool;

	RobotTileTable mRobotList; /**< List of active robots and their positions on the map. */

	InsertMode mInsertMode = InsertMode::None; /**< What's being inserted into the TileMap if anything. */
	StructureID mCurrentStructure = StructureID::SID_NONE; /**< Structure being placed. */
	Robot::Type mCurrentRobot = Robot::Type::None; /**< Robot being placed. */

	Population mPopulation;

	// USER INTERFACE
	Button mBtnTurns;
	Button mBtnToggleHeightmap;
	Button mBtnToggleConnectedness;
	Button mBtnToggleCommRangeOverlay;
	Button mBtnToggleRouteOverlay;
	Button mBtnTogglePoliceOverlay;

	// Bare Control's use for ToolTips
	Control mTooltipSystemButton;
	Control mTooltipCurrentTurns;

	ToolTip mToolTip;

	IconGrid mStructures{"ui/structures.png", 46, constants::MarginTight};
	IconGrid mRobots{"ui/robots.png", 46, constants::MarginTight};
	IconGrid mConnections{"ui/structures.png", 46, constants::MarginTight};

	DiggerDirection mDiggerDirection;
	FactoryProduction mFactoryProduction;
	FileIo mFileIoDialog;
	GameOverDialog mGameOverDialog;
	GameOptionsDialog mGameOptionsDialog;
	MajorEventAnnouncement mAnnouncement;
	MineOperationsWindow mMineOperationsWindow;
	NotificationArea mNotificationArea;
	NotificationWindow mNotificationWindow;
	PopulationPanel mPopulationPanel;
	ResourceBreakdownPanel mResourceBreakdownPanel;
	RobotInspector mRobotInspector;
	StructureInspector mStructureInspector;
	TileInspector mTileInspector;
	WarehouseInspector mWarehouseInspector;

	WindowStack mWindowStack;

	NAS2D::Rectangle<int> mBottomUiRect;

	// SIGNALS
	QuitSignal mQuitSignal;
	ReportsUiSignal mReportsUiSignal;
	MapChangedSignal mMapChangedSignal;

	// ROUTING
	micropather::MicroPather* mPathSolver = nullptr;

	// MISCELLANEOUS
	int mTurnCount = 0;

	int mCurrentMorale = constants::DefaultStartingMorale;
	int mPreviousMorale = constants::DefaultStartingMorale;

	int mLandersColonist = 0;
	int mLandersCargo = 0;

	int mResidentialCapacity = 0;

	std::vector<Tile*> mConnectednessOverlay;
	std::vector<Tile*> mCommRangeOverlay;
	std::vector<std::vector<Tile*>> mPoliceOverlays;
	std::vector<Tile*> mTruckRouteOverlay;

	bool mLoadingExisting = false;

	std::string mExistingToLoad; /**< Filename of the existing game to load. */

	ResourceInfoBar mResourceInfoBar;
	RobotDeploymentSummary mRobotDeploymentSummary;
	std::unique_ptr<MiniMap> mMiniMap;
	std::unique_ptr<DetailMap> mDetailMap;
	std::unique_ptr<NavControl> mNavControl;
};
