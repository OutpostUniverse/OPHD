#pragma once

#include <string>


/**
 * Defines string constants used throughout the game.
 *
 * The strings in this file are here to help translation/localization. These
 * are not UTF-8 strings and so only the first 255 values ASCII character
 * set are used.
 */
namespace constants
{
	// =====================================
	// = MISCELLANEOUS
	// =====================================
	const std::string Version = "v0.8.7";

	const std::string TileBulldozed = "Bulldozed";
	const std::string TileClear = "Clear";
	const std::string TileRough = "Rough";
	const std::string TileDifficult = "Difficult";
	const std::string TileImpassable = "Impassable";

	const std::string OreDepositYieldLow = "Low";
	const std::string OreDepositYieldMedium = "Medium";
	const std::string OreDepositYieldHigh = "High";

	const std::string ProductTransferTitle = "Remaining Product";
	const std::string ProductTransferMessage = "Bulldozing this Warehouse will result in some products being discarded due to insufficient storage space at other warehouses.\n\nDo you want to discard these products?";

	// =====================================
	// = SAVE GAMES
	// =====================================
	const std::string SaveGamePath = "savegames/";
	const std::string SaveGameVersion = "0.31";
	const std::string SaveGameRootNode = "OutpostHD_SaveGame";


	// =====================================
	// = RESOURCES
	// =====================================
	const std::string SaveGameResource0 = "resource_0";
	const std::string SaveGameResource1 = "resource_1";
	const std::string SaveGameResource2 = "resource_2";
	const std::string SaveGameResource3 = "resource_3";

	const std::string SaveGameEnergy = "energy";
	const std::string SaveGameFood = "food";


	// =====================================
	// = PRODUCT POOL
	// =====================================
	const std::string SaveGameProductDigger = "digger";
	const std::string SaveGameProductDozer = "dozer";
	const std::string SaveGameProductMiner = "miner";
	const std::string SaveGameProductExplorer = "explorer";
	const std::string SaveGameProductTruck = "truck";
	const std::string SaveGameProductMaintenanceParts = "maintenance_parts";

	const std::string SaveGameProductClothing = "clothing";
	const std::string SaveGameProductMedicine = "medicine";

	// =====================================
	// = FACTORY PRODUCTS
	// =====================================
	const std::string None = "None";
	const std::string Robodigger = "Robodigger";
	const std::string Robodozer = "Robodozer";
	const std::string Roboexplorer = "Roboexplorer";
	const std::string Robominer = "Robominer";
	const std::string Maintenance = "Maintenance";
	const std::string Truck = "Truck";

	const std::string Clothing = "Clothing";
	const std::string Medicine = "Medicine";


	// =====================================
	// = TUBE STATES
	// =====================================
	const std::string AgTubeIntersection = "ag_intersection";
	const std::string AgTubeRight = "ag_right";
	const std::string AgTubeLeft = "ag_left";

	const std::string UgTubeIntersection = "ug_intersection";
	const std::string UgTubeRight = "ug_right";
	const std::string UgTubeLeft = "ug_left";


	// =====================================
	// = STRUCTURE ANIMATION STATES
	// =====================================
	const std::string StructureStateConstruction = "construction";
	const std::string StructureStateOperational = "operational";
	const std::string StructureStateOperationalUg = "operational-ug";
	const std::string StructureStateDestroyed = "destroyed";


	// =====================================
	// = UI STRINGS
	// =====================================
	const std::string MainMenuNewGame = "New Game";
	const std::string MainMenuContinue = "Continue";
	const std::string MainMenuOptions = "Options";
	const std::string MainMenuHelp = "Help";
	const std::string MainMenuQuit = "Quit";

	const std::string WindowFactoryProduction = "Factory Production";
	const std::string WindowGameOver = "Game Over";
	const std::string WindowMineOperations = "Mine Facility Operations";
	const std::string WindowTileInspector = "Tile Inspector";
	const std::string WindowWarehouseInspector = "Warehouse Details";

	const std::string WindowFileIoTitleLoad = "Load Game";
	const std::string WindowFileIoTitleSave = "Save Game";
	const std::string WindowFileIoTitleDelete = "Delete Game";
	const std::string WindowFileIoLoad = "Load";
	const std::string WindowFileIoSave = "Save";
	const std::string WindowFileIoDelete = "Delete";

	const std::string WindowSystemTitle = "Options";

	// =====================================
	// = UI STRINGS
	// =====================================
	const std::string WarehouseEmpty = "Empty";
	const std::string WarehouseFull = "At Capacity";
	const std::string WarehouseVacancy = "Vacancy";

	const std::string ResearchReportTopicDetails = "Research Topic Details";

	// =====================================
	// = POPULATION PANEL STRINGS
	// =====================================
	const std::string PopulationBreakdown = "Population Breakdown";
	const std::string MoraleBreakdown = "Morale Breakdown";

	// =====================================
	// = POPULATION PANEL STRINGS
	// =====================================
	const std::string ToolTipBtnTurns = "Next turn";
	const std::string ToolTipBtnHeightmap = "Terrain Height Map";
	const std::string ToolTipBtnConnectedness = "Tube Connection Overlay";
	const std::string ToolTipBtnCommRange = "Communications Range Overlay";
	const std::string ToolTipBtnRoutes = "Ore Haul Routes Overlay";
	const std::string ToolTipBtnPolice = "Police Coverage Overlay";
	const std::string ToolTipRefinedResources = "Breakdown of storage levels for Smelted Resources";
	const std::string ToolTipResourceStorage = "Smelted Resources Storage. Current Level / Capacity";
	const std::string ToolTipFoodStorage = "Food Storage. Current Level / Capacity";
	const std::string ToolTipEnergy = "Energy Availability. Available / Total Production";
	const std::string ToolTipPopulation = "Population and Morale details";
	const std::string ToolTipCurrentTurns = "Current turn count";
	const std::string ToolTipSystemMenu = "System Menu";

	// =====================================
	// = UI STRINGS
	// =====================================
	const std::string TakeMeThere = "Take Me There";
	const std::string Idle = "Idle";
	const std::string AddTruck = "Add Truck";
	const std::string RemoveTruck = "Remove Truck";
	const std::string Cancel = "Cancel";

	// =====================================
	// = ALERT AND ERROR MESSAGES
	// =====================================
	const std::string AlertInvalidRobotPlacement = "Invalid Robot Action";
	const std::string AlertInvalidStructureAction = "Invalid Structure Action";

	const std::string AlertMineNotExhausted = "Mines can only be bulldozed after they have been fully extended and are completely exhausted.";
	const std::string AlertCannotBulldozeCc = "The Command Center cannot be bulldozed.";
	const std::string AlertCannotBulldozeLandingSite = "Landing sites cannot be bulldozed.";
	const std::string AlertTileBulldozed = "This tile has already been bulldozed.";

	const std::string AlertDiggerEdgeBuffer = Robodigger + "'s cannot be placed within 3 tiles of the edge of the site map.";
	const std::string AlertDiggerBlockedBelow = "The " + Robodigger + " cannot dig down because it is obstructed by something below.";
	const std::string AlertDiggerMineTile = "Destroy mine?";
	const std::string AlertDiggerMine = "The selected tile contains a mine. Placing a " + Robodigger + "here will destroy the mine. Do you want to place a " + Robodigger + " here?";

	const std::string AlertMinerTileObstructed = "Cannot place " + Robominer + " because there is an object on the selected tile.";
	const std::string AlertMinerSurfaceOnly = "The " + Robominer + " can only be placed on the surface.";
	const std::string AlertMinerNotOnMine = "The " + Robominer + " can only be placed on a marked Mine.";

	const std::string AlertMaxDigDepth = "The maximum digging depth for this planet has already been reached.";
	const std::string AlertStructureInWay = "A " + Robodigger + " cannot be placed on a Structure.";

	const std::string AlertOutOfCommRange = "The selected tile is out of communications range.";

	const std::string AlertLanderLocation = "Lander Location";
	const std::string AlertSeedTerrain = "The SEED Lander cannot be placed on or near Impassable terrain.";
	const std::string AlertSeedOreDeposit = "The SEED Lander cannot be placed on or near a tile flagged with a mine beacon.";
	const std::string AlertSeedEdgeBuffer = "SEED Lander's cannot be placed within 3 tiles of the edge of the site map.";

	const std::string AlertLanderTileObstructed = "Cannot place Lander because there is an object on the selected tile.";
	const std::string AlertLanderTerrain = "Landers cannot be placed on Impassable Terrain.";
	const std::string AlertLanderCommRange = "Landers must be placed within communications range.";

	const std::string AlertStructureOutOfRange = "Cannot build structures outside of Command Center communications range.";
	const std::string AlertStructureTileObstructed = "The selected tile already has a structure on it. You must bulldoze the existing structure in order to build here.";
	const std::string AlertStructureTileMapObject = "The selected tile is occupied by another object.";
	const std::string AlertStructureTerrain = "The selected tile is not bulldozed. Structures can only be built on bulldozed tiles.";
	const std::string AlertStructureOreDepositInWay = "The selected tile contains an ore deposit. Structures cannot be built on ore deposits.";
	const std::string AlertStructureExcavated = "Structures can only be placed on a tile that has been excavated and bulldozed.";
	const std::string AlertStructureNoTube = "The selected tile has no connection to the Command Center.";
	const std::string AlertStructureInsufficientResources = "You have insufficient resources to build this Structure. To build this structure, you will need an additional:\n\n";

	const std::string AlertTubeInvalidLocation = "Tubes can only be placed on bulldozed tiles that are adjacent to other Tubes and Structures that have a connection to the Command Center.";
	const std::string AlertTubeTerrain = "The selected tile is not bulldozed. Tubes can only be built on bulldozed tiles.";
}
