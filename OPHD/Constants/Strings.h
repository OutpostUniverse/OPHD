#pragma once

#include "../States/Planet.h"
#include "Numbers.h"

#include <string>
#include <map>


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
	const std::string VERSION = "v0.8.1";
	const std::string EMPTY_STR = ""; /**< Used in a few places to return an empty string without having to create one. */

	const std::map<Planet::PlanetType, std::string> PLANET_DESCRIPTIONS
	{
		{Planet::PlanetType::Ganymede, "Like Jupiter's moon Ganymede, this planet contains a sub-surface ocean and a magnetic field. Despite having a magnetic field indicating a liquid core, there is no atmosphere. \n \n This planet tends to be farther from the host star so solar satellites aren't going to be very useful."},
		{Planet::PlanetType::Mars, "This planet is a lot like our very own Mars. It is a relatively easy planet to colonize with a marginal atmosphere, lots of potential mining locations and you can dig deep. \n \n This leads to the problem that, while mines are plentiful, they don't yield a lot of resources."},
		{Planet::PlanetType::Mercury, "Small, hot and not very friendly. This planet resembles our very own Mercury. Close to its parent star, few potential mine locations, very shallow dig depth, no magnetic field and no atmosphere. \n \n The Payoff? The few mines you do get are higher yield and you get plenty of solar energy."}
	};

	const std::string TILE_INDEX_TRANSLATION_BULLDOZED = "Bulldozed";
	const std::string TILE_INDEX_TRANSLATION_CLEAR = "Clear";
	const std::string TILE_INDEX_TRANSLATION_ROUGH = "Rough";
	const std::string TILE_INDEX_TRANSLATION_DIFFICULT = "Difficult";
	const std::string TILE_INDEX_TRANSLATION_IMPASSABLE = "Impassable";

	const std::string MINE_YIELD_LOW = "Low";
	const std::string MINE_YIELD_MEDIUM = "Medium";
	const std::string MINE_YIELD_HIGH = "High";

	const std::string PRODUCT_TRANSFER_TITLE = "Remaining Product";
	const std::string PRODUCT_TRANSFER_MESSAGE = "Bulldozing this Warehouse will result in some products being discarded due to insufficient storage space at other warehouses.\n\nDo you want to discard these products?";

	// =====================================
	// = SAVE GAMES
	// =====================================
	const std::string SAVE_GAME_PATH = "savegames/";
	const std::string SAVE_GAME_VERSION = "0.31";
	const std::string SAVE_GAME_ROOT_NODE = "OutpostHD_SaveGame";


	// =====================================
	// = RESOURCES
	// =====================================
	const std::string SAVE_GAME_RESOURCE_0 = "resource_0";
	const std::string SAVE_GAME_RESOURCE_1 = "resource_1";
	const std::string SAVE_GAME_RESOURCE_2 = "resource_2";
	const std::string SAVE_GAME_RESOURCE_3 = "resource_3";

	const std::string SAVE_GAME_ENERGY = "energy";
	const std::string SAVE_GAME_FOOD = "food";


	// =====================================
	// = PRODUCT POOL
	// =====================================
	const std::string SAVE_GAME_PRODUCT_DIGGER = "digger";
	const std::string SAVE_GAME_PRODUCT_DOZER = "dozer";
	const std::string SAVE_GAME_PRODUCT_MINER = "miner";
	const std::string SAVE_GAME_PRODUCT_EXPLORER = "explorer";
	const std::string SAVE_GAME_PRODUCT_TRUCK = "truck";
	const std::string SAVE_GAME_PRODUCT_ROAD_MATERIALS = "road_materials";
	const std::string SAVE_GAME_MAINTENANCE_PARTS = "maintenance_parts";

	const std::string SAVE_GAME_PRODUCT_CLOTHING = "clothing";
	const std::string SAVE_GAME_PRODUCT_MEDICINE = "medicine";

	// =====================================
	// = FACTORY PRODUCTS
	// =====================================
	const std::string NONE = "None";
	const std::string ROBODIGGER = "Robodigger";
	const std::string ROBODOZER = "Robodozer";
	const std::string ROBOEXPLORER = "Roboexplorer";
	const std::string ROBOMINER = "Robominer";
	const std::string ROAD_MATERIALS = "Road Materials";
	const std::string MAINTENANCE_SUPPLIES = "Maintenance Supplies";
	const std::string TRUCK = "Truck";

	const std::string CLOTHING = "Clothing";
	const std::string MEDICINE = "Medicine";


	// =====================================
	// = STRUCTURES
	// =====================================
	const std::string AGRIDOME = "Agricultural Dome";
	const std::string AIR_SHAFT = "Air Shaft";
	const std::string CARGO_LANDER = "Cargo Lander";
	const std::string CHAP = "CHAP Facility";
	const std::string COLONIST_LANDER = "Colonist Lander";
	const std::string COMMAND_CENTER = "Command Center";
	const std::string COMMERCIAL = "Commercial";
	const std::string COMM_TOWER = "Communications Tower";
	const std::string FUSION_REACTOR = "Fusion Reactor";
	const std::string HOT_LABORATORY = "Hot Laboratory";
	const std::string LABORATORY = "Laboratory";
	const std::string MEDICAL_CENTER = "Medical Center";
	const std::string MINE_FACILITY = "Mine Facility";
	const std::string MINE_SHAFT = "Mine Shaft";
	const std::string NURSERY = "Nursery";
	const std::string PARK = "Park / Reservoir ";
	const std::string RECREATION_CENTER = "Recreation Center";
	const std::string RECYCLING = "Recycling Facility";
	const std::string RED_LIGHT_DISTRICT = "Red Light District";
	const std::string RESIDENCE = "Residential Facility";
	const std::string ROAD = "Road";
	const std::string ROBOT_COMMAND = "Robot Command";
	const std::string SEED_FACTORY = "SEED Factory";
	const std::string SEED_LANDER = "SEED Lander";
	const std::string SEED_POWER = "SEED Power";
	const std::string SEED_SMELTER = "SEED Smelter";
	const std::string SMELTER = "Smelter";
	const std::string SOLAR_PLANT = "Solar Powersat Receiver Array";
	const std::string SOLAR_PANEL1 = "Solar Panel Array";
	const std::string STORAGE_TANKS = "Storage Tanks";
	const std::string SURFACE_FACTORY = "Surface Factory";
	const std::string SURFACE_POLICE = "Police";
	const std::string TUBE = "Tube";
	const std::string UNDERGROUND_FACTORY = "Underground Factory";
	const std::string UNDERGROUND_POLICE = "Police (UG)";
	const std::string UNIVERSITY = "University";
	const std::string WAREHOUSE = "Warehouse";


	// =====================================
	// = TUBE STATES
	// =====================================
	const std::string AG_TUBE_INTERSECTION = "ag_intersection";
	const std::string AG_TUBE_RIGHT = "ag_right";
	const std::string AG_TUBE_LEFT = "ag_left";

	const std::string UG_TUBE_INTERSECTION = "ug_intersection";
	const std::string UG_TUBE_RIGHT = "ug_right";
	const std::string UG_TUBE_LEFT = "ug_left";


	// =====================================
	// = SUB LEVEL STRINGS
	// =====================================
	const std::string LEVEL_SURFACE = "Surface";
	const std::string LEVEL_UG1 = "Underground 1";
	const std::string LEVEL_UG2 = "Underground 2";
	const std::string LEVEL_UG3 = "Underground 3";
	const std::string LEVEL_UG4 = "Underground 4";


	// =====================================
	// = STRUCTURE ANIMATION STATES
	// =====================================
	const std::string STRUCTURE_STATE_CONSTRUCTION = "construction";
	const std::string STRUCTURE_STATE_OPERATIONAL = "operational";
	const std::string STRUCTURE_STATE_OPERATIONAL_UG = "operational-ug";
	const std::string STRUCTURE_STATE_DESTROYED = "destroyed";


	// =====================================
	// = UI STRINGS
	// =====================================
	const std::string MAIN_MENU_NEW_GAME = "New Game";
	const std::string MAIN_MENU_CONTINUE = "Continue";
	const std::string MAIN_MENU_OPTIONS = "Options";
	const std::string MAIN_MENU_HELP = "Help";
	const std::string MAIN_MENU_QUIT = "Quit";

	const std::string DIFFICULTY_TITLE = "Difficulty";
	const std::string DIFFICULTY_BEGINNER = "Beginner";
	const std::string DIFFICULTY_EASY = "Easy";
	const std::string DIFFICULTY_MEDIUM = "Medium";
	const std::string DIFFICULTY_HARD = "Hard";

	const std::string WINDOW_FACTORY_PRODUCTION = "Factory Production";
	const std::string WINDOW_GAME_OVER = "Game Over";
	const std::string WINDOW_MINE_OPERATIONS = "Mine Facility Operations";
	const std::string WINDOW_STRUCTURE_INSPECTOR = "Structure Details";
	const std::string WINDOW_TILE_INSPECTOR = "Tile Inspector";
	const std::string WINDOW_WH_INSPECTOR = "Warehouse Details";

	const std::string WINDOW_FILEIO_TITLE_LOAD = "Load Game";
	const std::string WINDOW_FILEIO_TITLE_SAVE = "Save Game";
	const std::string WINDOW_FILEIO_TITLE_DELETE = "Delete Game";
	const std::string WINDOW_FILEIO_LOAD = "Load";
	const std::string WINDOW_FILEIO_SAVE = "Save";
	const std::string WINDOW_FILEIO_DELETE = "Delete";

	const std::string WINDOW_SYSTEM_TITLE = "Options";

	// =====================================
	// = UI STRINGS
	// =====================================
	const std::string WAREHOUSE_EMPTY = "Empty";
	const std::string WAREHOUSE_FULL = "At Capacity";
	const std::string WAREHOUSE_SPACE_AVAILABLE = "Space Available";

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
	const std::string BUTTON_TAKE_ME_THERE = "Take Me There";
	const std::string BUTTON_IDLE = "Idle";
	const std::string BUTTON_ADD_TRUCK = "Add Truck";
	const std::string BUTTON_REMOVE_TRUCK = "Remove Truck";
	const std::string ButtonCancel = "Cancel";

	// =====================================
	// = STRUCTURE DISABLED REASONS
	// =====================================
	const std::string STRUCTURE_DISABLED_NONE = "Not Disabled";
	const std::string STRUCTURE_DISABLED_CHAP = "CHAP Facility unavailable";
	const std::string STRUCTURE_DISABLED_DISCONNECTED = "Not connected to a Command Center";
	const std::string STRUCTURE_DISABLED_ENERGY = "Insufficient Energy";
	const std::string STRUCTURE_DISABLED_POPULATION = "Insufficient Population";
	const std::string STRUCTURE_DISABLED_REFINED_RESOURCES = "Insufficient refined resources";
	const std::string STRUCTURE_DISABLED_STRUCTURAL_INTEGRITY = "Structural Integrity below operating threshold";

	// =====================================
	// = STRUCTURE IDLE REASONS
	// =====================================
	const std::string STRUCTURE_IDLE_NONE = "Not Idle";
	const std::string STRUCTURE_IDLE_PLAYER_SET = "Manually set to Idle";
	const std::string STRUCTURE_IDLE_INTERNAL_STORAGE_FULL = "Internal storage pool full";
	const std::string STRUCTURE_IDLE_FACTORY_PRODUCTION_COMPLETE = "Production complete, waiting on product pull.";
	const std::string STRUCTURE_IDLE_FACTORY_INSUFFICIENT_RESOURCES = "Insufficient resources to continue production";
	const std::string STRUCTURE_IDLE_FACTORY_INSUFFICIENT_ROBOT_COMMAND_CAPACITY = "Cannot pull robot due to lack of robot command capacity";
	const std::string STRUCTURE_IDLE_FACTORY_INSUFFICIENT_WAREHOUSE_SPACE = "Cannot pull product due to lack of Warehouse space";
	const std::string STRUCTURE_IDLE_MINE_EXHAUSTED = "Mine exhausted";
	const std::string STRUCTURE_IDLE_MINE_INACTIVE = "Mine inactive";
	const std::string STRUCTURE_IDLE_INSUFFICIENT_LUXURY_PRODUCT = "Insufficient Luxury Product available";

	// =====================================
	// = ALERT AND ERROR MESSAGES
	// =====================================
	const std::string ALERT_INVALID_ROBOT_PLACEMENT = "Invalid Robot Action";
	const std::string ALERT_INVALID_STRUCTURE_ACTION = "Invalid Structure Action";

	const std::string ALERT_MINE_NOT_EXHAUSTED = "Mines can only be bulldozed after they have been fully extended and are completely exhausted.";
	const std::string ALERT_CANNOT_BULLDOZE_CC = "The Command Center cannot be bulldozed.";
	const std::string ALERT_CANNOT_BULLDOZE_LANDING_SITE = "Landing sites cannot be bulldozed.";
	const std::string ALERT_TILE_BULLDOZED = "This tile has already been bulldozed.";

	const std::string ALERT_DIGGER_EDGE_BUFFER = ROBODIGGER + "'s cannot be placed within 3 tiles of the edge of the site map.";
	const std::string ALERT_DIGGER_BLOCKED_BELOW = "The " + ROBODIGGER + " cannot dig down because it is obstructed by something below.";
	const std::string ALERT_DIGGER_MINE_TITLE = "Destroy mine?";
	const std::string ALERT_DIGGER_MINE = "The selected tile contains a mine. Placing a " + ROBODIGGER + "here will destroy the mine. Do you want to place a " + ROBODIGGER + " here?";

	const std::string ALERT_MINER_TILE_OBSTRUCTED = "Cannot place " + ROBOMINER + " because there is an object on the selected tile.";
	const std::string ALERT_MINER_SURFACE_ONLY = "The " + ROBOMINER + " can only be placed on the surface.";
	const std::string ALERT_MINER_NOT_ON_MINE = "The " + ROBOMINER + " can only be placed on a marked Mine.";

	const std::string ALERT_MAX_DIG_DEPTH = "The maximum digging depth for this planet has already been reached.";
	const std::string ALERT_STRUCTURE_IN_WAY = "A " + ROBODIGGER + " cannot be placed on a Structure.";

	const std::string ALERT_OUT_OF_COMM_RANGE = "The selected tile is out of communications range.";

	const std::string ALERT_LANDER_LOCATION = "Lander Location";
	const std::string ALERT_SEED_TERRAIN = "The " + SEED_LANDER + " cannot be placed on or near Impassable terrain.";
	const std::string ALERT_SEED_MINE = "The " + SEED_LANDER + " cannot be placed on or near a tile flagged with a Mine.";
	const std::string ALERT_SEED_EDGE_BUFFER = SEED_LANDER + "'s cannot be placed within 3 tiles of the edge of the site map.";

	const std::string ALERT_LANDER_TILE_OBSTRUCTED = "Cannot place Lander because there is an object on the selected tile.";
	const std::string ALERT_LANDER_TERRAIN = "Landers cannot be placed on Impassable Terrain.";
	const std::string ALERT_LANDER_COMM_RANGE = "Landers must be placed within 5 tiles of the Command Center.";

	const std::string ALERT_STRUCTURE_OUT_OF_RANGE = "Cannot build structures more than " + std::to_string(constants::ROBOT_COM_RANGE) + " tiles away from Command Center.";
	const std::string ALERT_STRUCTURE_TILE_OBSTRUCTED = "The selected tile already has a structure on it. You must bulldoze the existing structure in order to build here.";
	const std::string ALERT_STRUCTURE_TILE_THING = "The selected tile is occupied by another object.";
	const std::string ALERT_STRUCTURE_TERRAIN = "The selected tile is not bulldozed. Structures can only be built on bulldozed tiles.";
	const std::string ALERT_STRUCTURE_MINE_IN_WAY = "The selected tile contains a Mine. Structures cannot be built on Mines.";
	const std::string ALERT_STRUCTURE_EXCAVATED = "Structures can only be placed on a tile that has been excavated and bulldozed.";
	const std::string ALERT_STRUCTURE_NO_TUBE = "The selected tile has no connection to the Command Center.";
	const std::string ALERT_STRUCTURE_INSUFFICIENT_RESORUCES = "You have insufficient resources to build this Structure. To build this structure, you will need an additional:\n\n";

	const std::string ALERT_TUBE_INVALID_LOCATION = "Tubes can only be placed on bulldozed tiles that are adjacent to other Tubes and Structures that have a connection to the Command Center.";
}
