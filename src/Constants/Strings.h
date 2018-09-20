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
	const std::string VERSION = "v0.7.6";
	const std::string EMPTY_STR = ""; /**< Used in a few places to return an empty string without having to create one. */

	const std::string PLANET_DESCRIPTION_GANYMEDE = "Like Jupiter's moon Ganymede, this planet contains a sub-surface ocean and a magnetic field. Despite having a magnetic field indicating a liquid core, there is no atpmosphere. \n \n This planet tends to be farther from the host star so solar satellites aren't going to be very useful.";
	const std::string PLANET_DESCRIPTION_MARS = "This planet is a lot like our very own Mars. It is a relatively easy planet to colonize with a marginal atmosphere, lots of potential mining locations and you can dig deep. \n \n This leads to the problem that, while mines are plentiful, they don't yield a lot of resources.";
	const std::string PLANET_DESCRIPTION_MERCURY = "Small, hot and not very friendly. This planet resembles our very own Mercury. Close to its parent star, few potential mine locations, very shallow dig depth, no magnetic field and no atmpsphere. \n \n The Payoff? The few mines you do get are higher yield and you get plenty of solar energy.";

	const std::string TILE_INDEX_TRANSLATION_BULLDOZED = "Bulldozed";
	const std::string TILE_INDEX_TRANSLATION_CLEAR = "Clear";
	const std::string TILE_INDEX_TRANSLATION_ROUGH = "Rough";
	const std::string TILE_INDEX_TRANSLATION_DIFFICULT = "Difficult";
	const std::string TILE_INDEX_TRANSLATION_IMPASSABLE = "Impassable";

	const std::string MINE_YIELD_LOW = "Low";
	const std::string MINE_YIELD_MEDIUM = "Medium";
	const std::string MINE_YIELD_HIGH = "High";

	// =====================================
	// = SAVE GAMES
	// =====================================
	const std::string SAVE_GAME_PATH = "savegames/";
	const std::string SAVE_GAME_VERSION = "0.30";
	const std::string SAVE_GAME_ROOT_NODE = "OutpostHD_SaveGame";


	// =====================================
	// = RESOURCES
	// =====================================
	const std::string SAVE_GAME_COMMON_METAL_ORE = "common_metal_ore";
	const std::string SAVE_GAME_COMMON_MINERAL_ORE = "common_mineral_ore";
	const std::string SAVE_GAME_RARE_METAL_ORE = "rare_metal_ore";
	const std::string SAVE_GAME_RARE_MINERAL_ORE = "rare_mineral_ore";

	const std::string SAVE_GAME_COMMON_METAL = "common_metal";
	const std::string SAVE_GAME_COMMON_MINERAL = "common_mineral";
	const std::string SAVE_GAME_RARE_METAL = "rare_metal";
	const std::string SAVE_GAME_RARE_MINERAL = "rare_mineral";

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
	const std::string RED_LIGHT_DISTRICT = "Red Light District";
	const std::string RESIDENCE = "Residential Facility";
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
	// = PLANET SPRITE SHEETS
	// =====================================
	const std::string PLANET_TYPE_MERCURY_PATH = "planets/planet_d.png";
	const std::string PLANET_TYPE_MARS_PATH = "planets/planet_c.png";
	const std::string PLANET_TYPE_GANYMEDE_PATH = "planets/planet_e.png";


	// =====================================
	// = UI STRINGS
	// =====================================
	const std::string MAIN_MENU_NEW_GAME = "New Game";
	const std::string MAIN_MENU_CONTINUE = "Continue";
	const std::string MAIN_MENU_OPTIONS = "Options";
	const std::string MAIN_MENU_QUIT = "Quit";

	const std::string WINDOW_FACTORY_PRODUCTION = "Factory Production";
	const std::string WINDOW_GAME_OVER = "Game Over";
	const std::string WINDOW_MINE_OPERATIONS = "Mine Facility Operations";
	const std::string WINDOW_STRUCTURE_INSPECTOR = "Structure Inspector";
	const std::string WINDOW_TILE_INSPECTOR = "Tile Inspector";
	const std::string WINDOW_WH_INSPECTOR = "Warehouse Inspector";

	const std::string WINDOW_FILEIO_TITLE_LOAD = "Load Game";
	const std::string WINDOW_FILEIO_TITLE_SAVE = "Save Game";
	const std::string WINDOW_FILEIO_LOAD = "Load";
	const std::string WINDOW_FILEIO_SAVE = "Save";

	const std::string WINDOW_SYSTEM_TITLE = "Options";
}
