#pragma once

#include "Wrapper.h"
#include "CrimeRateUpdate.h"
#include "CrimeExecution.h"
#include "StructureTracker.h"

#include "../RobotPool.h"

#include "../UI/NotificationArea.h"
#include "../UI/NotificationWindow.h"
#include "../UI/DiggerDirection.h"
#include "../UI/FactoryProduction.h"
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
#include "../UI/CheatMenu.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/EnumStructureID.h>
#include <libOPHD/PlanetAttributes.h>
#include <libOPHD/StorableResources.h>

#include <libOPHD/Map/MapCoordinate.h>

#include <libOPHD/Population/PopulationPool.h>
#include <libOPHD/Population/Population.h>
#include <libOPHD/Population/Morale.h>

#include <libOPHD/Technology/ResearchTracker.h>
#include <libOPHD/Technology/TechnologyCatalog.h>

#include <libControls/WindowStack.h>
#include <libControls/ToolTip.h>

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Fade.h>

#include <string>
#include <memory>
#include <map>


namespace NAS2D
{
	namespace Xml
	{
		class XmlDocument;
		class XmlElement;
	}

	template <typename BaseType> struct Point;
}

namespace micropather
{
	class MicroPather;
}

enum class Direction;
enum class RobotTypeIndex;

class Structure;
class Tile;
class TileMap;
class MapView;
class DetailMap;
class MiniMap;
class NavControl;
class ReportsState;
class GameState;
class FileIo;
class ColonyShip;


enum class InsertMode
{
	None,
	Robot,
	Tube,
	Structure
};

using RobotTileTable = std::map<Robot*, Tile*>;


class MapViewState : public Wrapper
{
public:
	enum class PopulationLevel
	{
		Small = 1,
		Large = 2
	};

public:
	using EventDelegate = NAS2D::Delegate<void()>;

public:
	MapViewState(GameState& gameState, NAS2D::Xml::XmlDocument& saveGameDocument, EventDelegate quitHandler);
	MapViewState(GameState& gameState, const PlanetAttributes& planetAttributes, Difficulty selectedDifficulty, EventDelegate quitHandler);
	~MapViewState() override;

	void mapChangedHandler(EventDelegate newMapChangedHandler) { mMapChangedHandler = newMapChangedHandler; }

	void focusOnStructure(const Structure* s);

	Difficulty difficulty() { return mDifficulty; }
	void difficulty(Difficulty difficulty);

	bool hasGameEnded();

	void initialize() override;
	State* update() override;

	void save(NAS2D::Xml::XmlDocument&);

private:
	void onDeactivate() override;
	void onActivate() override;

	// EVENT HANDLERS
	void onActivate(bool newActiveValue);
	void onKeyDown(NAS2D::KeyCode key, NAS2D::KeyModifier mod, bool repeat);
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseDoubleClick(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);
	void onMouseWheel(NAS2D::Vector<int> changeAmount);
	void onWindowResized(NAS2D::Vector<int> newSize);

	void onInspect(const MapCoordinate& tilePosition, bool inspectModifier);
	void onInspectStructure(Structure& structure, bool inspectModifier);
	void onInspectRobot(Robot& robot);
	void onInspectTile(Tile& tile);

	void onClickMap();

	void onSystemMenu();

	// ROBOT EVENT HANDLERS
	void onDozerTaskComplete(Robot& robot);
	void onDiggerTaskComplete(Robot& robot);
	void onMinerTaskComplete(Robot& robot);

	// DRAWING FUNCTIONS
	void drawUI();
	void drawSystemButton() const;

	// INSERT OBJECT HANDLING
	void onDeployCargoLander();
	void onDeployColonistLander();
	void onDeploySeedLander(NAS2D::Point<int> point);
	void insertSeedLander(NAS2D::Point<int> point);
	void insertTube(ConnectorDir dir, int depth, Tile& tile);

	void placeTubes(Tile& tile, ConnectorDir connectorDirection);
	void placeStructure(Tile& tile, StructureID structureID);
	void placeRobot(Tile& tile, RobotTypeIndex robotTypeIndex);

	void placeRobodozer(Tile&);
	void placeRobodigger(Tile&);
	void placeRobominer(Tile&);

	Robot& addRobot(RobotTypeIndex robotTypeIndex);

	// MISCELLANEOUS UTILITY FUNCTIONS
	void updateFood();
	void transferFoodToCommandCenter();

	void updateCommRangeOverlay();
	void updatePoliceOverlay();
	void updateConnectedness();
	void changeViewDepth(int);
	void moveView(MapOffset offset);
	void onChangeDepth(int oldDepth, int newDepth);

	void pullRobotFromFactory(ProductType pt, Factory& factory);
	void onFactoryProductionComplete(Factory& factory);
	void onCheatCodeEntry(const std::string& cheatCode);

	void onMineFacilityExtend(MineFacility* mf);

	void onCrimeEvent(std::string title, std::string text, const Structure& structure);

	void updatePlayerResources();
	void updateResearch();

	// TURN LOGIC
	void checkColonyShip();
	void onColonyShipCrash(const ColonyShipLanders&);
	void checkWarehouseCapacity();
	void nextTurn();
	void updatePopulation();
	void updateCommercial();
	void updateMaintenance();
	void updateMorale();
	void notifyBirthsAndDeaths();
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
	void readRobots(NAS2D::Xml::XmlElement* element);
	void readStructures(NAS2D::Xml::XmlElement* element);
	void readTurns(NAS2D::Xml::XmlElement* element);
	void readPopulation(NAS2D::Xml::XmlElement* element);
	void readMoraleChanges(NAS2D::Xml::XmlElement*);

	void scrubRobotList();

	void load(NAS2D::Xml::XmlDocument*);
	NAS2D::Xml::XmlElement* serializeProperties();

	// UI MANAGEMENT FUNCTIONS
	bool isInserting() const;
	bool isInsertingRobot() const;
	void clearBuildMode();
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

	void onOpenSaveGameDialog();
	void onOpenLoadGameDialog();
	void onReturnToGame();
	void onGameOver();

	void onMapObjectSelectionChanged();
	void onStructuresSelectionChange(const IconGridItem*);
	void onConnectionsSelectionChange(const IconGridItem*);
	void onRobotsSelectionChange(const IconGridItem*);

	void onDiggerSelectionDialog(Direction direction, Tile& tile);

	void onTakeMeThere(const MapCoordinate& position);

private:
	Difficulty mDifficulty = Difficulty::Medium;
	std::unique_ptr<TileMap> mTileMap;
	CrimeRateUpdate mCrimeRateUpdate;
	CrimeExecution mCrimeExecution;

	ColonyShip& mColonyShip;
	StructureTracker mStructureTracker;

	ResearchTracker mResearchTracker;
	TechnologyCatalog mTechnologyReader;

	PlanetAttributes mPlanetAttributes;

	int mFood{0};

	// MISCELLANEOUS
	int mTurnCount = 0;

	int mTurnNumberOfLanding; /**< First turn that human colonists landed. */

	Morale mMorale;

	int mResidentialCapacity = 0;

	// POOLS
	StorableResources mResourcesCount;
	RobotPool mRobotPool; /**< Robots that are currently available for use. */
	PopulationPool mPopulationPool;

	RobotTileTable mRobotList; /**< List of active robots and their positions on the map. */
	Population mPopulation;

	// ROUTING
	std::unique_ptr<micropather::MicroPather> mPathSolver;

	bool mLoadingExisting = false;
	NAS2D::Xml::XmlDocument* mExistingToLoad = nullptr; 

	ReportsState& mReportsState;
	std::unique_ptr<MapView> mMapView;

	const NAS2D::Image mUiIcons{"ui/icons.png"}; /**< User interface icons. */
	const NAS2D::Image mBackground{"sys/bg1.png"}; /**< Background image drawn behind the tile map. */

	NAS2D::Rectangle<int> mMiniMapRect; /**< Area of the site map display. */

	// Map Object picking
	InsertMode mInsertMode = InsertMode::None; /**< What's being inserted into the TileMap if anything. */
	StructureID mCurrentStructure = StructureID::SID_NONE; /**< Structure being placed. */
	RobotTypeIndex mCurrentRobot; /**< Robot being placed. */
	IconGrid mStructures;
	IconGrid mRobots;
	IconGrid mConnections;

	// USER INTERFACE
	Button mBtnTurns;
	Button mBtnToggleHeightmap;
	Button mBtnToggleRouteOverlay;
	Button mBtnToggleConnectedness;
	Button mBtnToggleCommRangeOverlay;
	Button mBtnTogglePoliceOverlay;

	// Bare Control's use for ToolTips
	Control mTooltipSystemButton;
	Control mTooltipCurrentTurns;

	ToolTip mToolTip;

	CheatMenu mCheatMenu;
	DiggerDirection mDiggerDirection;
	FactoryProduction mFactoryProduction;
	FileIo& mFileIoDialog;
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
	EventDelegate mQuitHandler;
	EventDelegate mMapChangedHandler;

	std::vector<Tile*> mConnectednessOverlay;
	std::vector<Tile*> mCommRangeOverlay;
	std::vector<std::vector<Tile*>> mPoliceOverlays;
	std::vector<Tile*> mTruckRouteOverlay;

	ResourceInfoBar mResourceInfoBar;
	RobotDeploymentSummary mRobotDeploymentSummary;
	std::unique_ptr<MiniMap> mMiniMap;
	std::unique_ptr<DetailMap> mDetailMap;
	std::unique_ptr<NavControl> mNavControl;

	NAS2D::Fade mFade;
};
