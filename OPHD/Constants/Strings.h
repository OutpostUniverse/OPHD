#pragma once

#include "Numbers.h"

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

	const std::string MineYieldLow = "Low";
	const std::string MineYieldMedium = "Medium";
	const std::string MineYieldHigh = "High";

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
	const std::string MaintenanceSupplies = "Maintenance Supplies";
	const std::string Truck = "Truck";

	const std::string Clothing = "Clothing";
	const std::string Medicine = "Medicine";


	// =====================================
	// = STRUCTURES
	// =====================================
	const std::string Agridome = "Agricultural Dome";
	const std::string AirShaft = "Air Shaft";
	const std::string CargoLander = "Cargo Lander";
	const std::string Chap = "CHAP Facility";
	const std::string ColonistLander = "Colonist Lander";
	const std::string CommandCenter = "Command Center";
	const std::string Commercial = "Commercial";
	const std::string CommTower = "Communications Tower";
	const std::string FusionReactor = "Fusion Reactor";
	const std::string HotLaboratory = "Hot Laboratory";
	const std::string Laboratory = "Laboratory";
	const std::string MaintenanceFacility = "Maintenance Facility";
	const std::string MedicalCenter = "Medical Center";
	const std::string MineFacility = "Mine Facility";
	const std::string MineShaft = "Mine Shaft";
	const std::string Nursery = "Nursery";
	const std::string Park = "Park / Reservoir";
	const std::string RecreationCenter = "Recreation Center";
	const std::string Recycling = "Recycling Facility";
	const std::string RedLightDistrict = "Red Light District";
	const std::string Residence = "Residential Facility";
	const std::string Road = "Road";
	const std::string RobotCommand = "Robot Command";
	const std::string SeedFactory = "SEED Factory";
	const std::string SeedLander = "SEED Lander";
	const std::string SeedPower = "SEED Power";
	const std::string SeedSmelter = "SEED Smelter";
	const std::string Smelter = "Smelter";
	const std::string SolarPlant = "Solar Powersat Receiver Array";
	const std::string SolarPanel1 = "Solar Panel Array";
	const std::string StorageTanks = "Storage Tanks";
	const std::string SurfaceFactory = "Surface Factory";
	const std::string SurfacePolice = "Police";
	const std::string Tube = "Tube";
	const std::string UndergroundFactory = "Underground Factory";
	const std::string UndergroundPolice = "Police (UG)";
	const std::string University = "University";
	const std::string Warehouse = "Warehouse";


	// =====================================
	// = TUBE STATES
	// =====================================
	const std::string AgTubeIntersection = "ag_intersection";
	const std::string AgTubeRight = "ag_right";
	const std::string AgTubeLeft = "ag_left";

	const std::string UgTubeIntersection = "ug_intersection";
	const std::string UgTubeRight = "ug_right";
	const std::string UgTubelLeft = "ug_left";


	// =====================================
	// = SUB LEVEL STRINGS
	// =====================================
	const std::string LevelSurface = "Surface";
	const std::string Levelunderground1 = "Underground 1";
	const std::string Levelunderground2 = "Underground 2";
	const std::string Levelunderground3 = "Underground 3";
	const std::string Levelunderground4 = "Underground 4";


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
	const std::string WindowStructureInspector = "Structure Details";
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
	const std::string WarehouseSpaceAvailable = "Space Available";

	// =====================================
	// = POPULATION PANEL STRINGS
	// =====================================
	const std::string PopulationBreakdown = "Population Breakdown";
	const std::string MoraleBreakdown = "Morale Breakdown";

	// =====================================
	// = POPULATION PANEL STRINGS
	// =====================================
	const std::string ToolTipBtnTurns = "Advances to the next turn";
	const std::string ToolTipBtnHeightmap = "Toggles display of the Terrain Height Map";
	const std::string ToolTipBtnConnectedness = "Toggles display of the Tile Connection Overlay";
	const std::string ToolTipBtnCommRange = "Toggles display of the Communications Range Overlay";
	const std::string ToolTipBtnRoutes = "Toggles display of the Trucking Routes Overlay";
	const std::string ToolTipBtnPolice = "Toggles display of the Police Station Coverage";
	const std::string ToolTipRefinedResources = "Breakdown of storage levels for Smelted Resources";
	const std::string ToolTipResourceStorage = "Smelted Resources Storage. Current Level / Capacity";
	const std::string ToolTipFoodStorage = "Food Storage. Current Level / Capacity";
	const std::string ToolTipEnergy = "Energy Availability. Available / Total Production";
	const std::string ToolTipPopulation = "Population and Morale details";
	const std::string ToolTipCurrentTurns = "Current turn count";
	const std::string ToolTipSystemMenu = "System Menu. Options for saving, loading and etc";

	// =====================================
	// = UI STRINGS
	// =====================================
	const std::string TakeMeThere = "Take Me There";
	const std::string Idle = "Idle";
	const std::string AddTruck = "Add Truck";
	const std::string RemoveTruck = "Remove Truck";
	const std::string Cancel = "Cancel";

	// =====================================
	// = STRUCTURE DISABLED REASONS
	// =====================================
	const std::string StructureDisabledNone = "Not Disabled";
	const std::string StructureDisabledChap = "CHAP Facility unavailable";
	const std::string StructureDisabledDisconnected = "Not connected to a Command Center";
	const std::string StructureDisabledEnergy = "Insufficient Energy";
	const std::string StructureDisabledPopulation = "Insufficient Population";
	const std::string StructureDisabledRefinedResources = "Insufficient refined resources";
	const std::string StructureDisabledStructuralIntegrity = "Structural Integrity is compromised";

	// =====================================
	// = STRUCTURE IDLE REASONS
	// =====================================
	const std::string StructureIdleNone = "Not Idle";
	const std::string StructureIdlePlayerSet = "Manually set to Idle";
	const std::string StructureIdleInternalStorageFull = "Internal storage pool full";
	const std::string StructureIdleFactoryProductionComplete = "Production complete, waiting on product pull.";
	const std::string StructureIdleFactoryInsufficientResources = "Insufficient resources to continue production";
	const std::string StructureIdleFactoryInsufficientRobotCommandCapacity = "Cannot pull robot due to lack of robot command capacity";
	const std::string StructureIdleFactoryInsufficnetWarehouseCapacity = "Cannot pull product due to lack of Warehouse space";
	const std::string StructureIdleMineExhausted = "Mine exhausted";
	const std::string StructureIdleMineInactive = "Mine inactive";
	const std::string StructureIdleInsufficientLuxuryProduct = "Insufficient Luxury Product available";

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
	const std::string AlertSeedTerrain = "The " + SeedLander + " cannot be placed on or near Impassable terrain.";
	const std::string AlertSeedMine = "The " + SeedLander + " cannot be placed on or near a tile flagged with a Mine.";
	const std::string AlertSeedEdgeBuffer = SeedLander + "'s cannot be placed within 3 tiles of the edge of the site map.";

	const std::string AlertLanderTileObstructed = "Cannot place Lander because there is an object on the selected tile.";
	const std::string AlertLanderTerrain = "Landers cannot be placed on Impassable Terrain.";
	const std::string AlertLanderCommRange = "Landers must be placed within 5 tiles of the Command Center.";

	const std::string AlertStructureOutOfRange = "Cannot build structures more than " + std::to_string(RobotCommRange) + " tiles away from Command Center.";
	const std::string AlertStructureTileObstructed = "The selected tile already has a structure on it. You must bulldoze the existing structure in order to build here.";
	const std::string AlertStructureTileMapObject = "The selected tile is occupied by another object.";
	const std::string AlertStructureTerrain = "The selected tile is not bulldozed. Structures can only be built on bulldozed tiles.";
	const std::string AlertStructureMineInWay = "The selected tile contains a Mine. Structures cannot be built on Mines.";
	const std::string AlertStructureExcavated = "Structures can only be placed on a tile that has been excavated and bulldozed.";
	const std::string AlertStructureNoTube = "The selected tile has no connection to the Command Center.";
	const std::string AlertStructureInsufficientResources = "You have insufficient resources to build this Structure. To build this structure, you will need an additional:\n\n";

	const std::string AlertTubeInvalidLocation = "Tubes can only be placed on bulldozed tiles that are adjacent to other Tubes and Structures that have a connection to the Command Center.";
	const std::string AlertTubeTerrain = "The selected tile is not bulldozed. Tubes can only be built on bulldozed tiles.";
}
