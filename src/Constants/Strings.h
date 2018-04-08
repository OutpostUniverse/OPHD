#pragma once

#include <iostream>

using namespace std;

namespace constants
{
	// MISCELLANEOUS
	const string	VERSION = "v0.7.5";
	const string	EMPTY_STR = "";

	const string	PLANET_DESCRIPTION_GANYMEDE = "Like Jupiter's moon Ganymede, this planet contains a sub-surface ocean and a magnetic field. Despite having a magnetic field indicating a liquid core, there is no atpmosphere. \n \n This planet tends to be farther from the host star so solar satellites aren't going to be very useful.";
	const string	PLANET_DESCRIPTION_MARS = "This planet is a lot like our very own Mars. It is a relatively easy planet to colonize with a marginal atmosphere, lots of potential mining locations and you can dig deep. \n \n This leads to the problem that, while mines are plentiful, they don't yield a lot of resources.";
	const string	PLANET_DESCRIPTION_MERCURY = "Small, hot and not very friendly. This planet resembles our very own Mercury. Close to its parent star, few potential mine locations, very shallow dig depth, no magnetic field and no atmpsphere. \n \n The Payoff? The few mines you do get are higher yield and you get plenty of solar energy.";

	// SAVE GAMES
	const string	SAVE_GAME_PATH = "savegames/";
	const string	SAVE_GAME_VERSION = "0.25";
	const string	SAVE_GAME_ROOT_NODE = "OutpostHD_SaveGame";

	const string	SAVE_GAME_COMMON_METAL_ORE = "common_metal_ore";
	const string	SAVE_GAME_COMMON_MINERAL_ORE = "common_mineral_ore";
	const string	SAVE_GAME_RARE_METAL_ORE = "rare_metal_ore";
	const string	SAVE_GAME_RARE_MINERAL_ORE = "rare_mineral_ore";

	const string	SAVE_GAME_COMMON_METAL = "common_metal";
	const string	SAVE_GAME_COMMON_MINERAL = "common_mineral";
	const string	SAVE_GAME_RARE_METAL = "rare_metal";
	const string	SAVE_GAME_RARE_MINERAL = "rare_mineral";

	const string	SAVE_GAME_ENERGY = "energy";
	const string	SAVE_GAME_FOOD = "food";

	// ROBOTS
	const string	ROBODIGGER = "Robodigger";
	const string	ROBODOZER = "Robodozer";
	const string	ROBOEXPLORER = "Roboexplorer";
	const string	ROBOMINER = "Robominer";
	const string	ROAD_MATERIALS = "Road Materials";
	const string	MAINTENANCE_SUPPLIES = "Maintenance Supplies";

	const string	TRUCK = "Truck";

	const string	CLOTHING = "Clothing";
	const string	MEDICINE = "Medicine";


	// STRUCTURES
	const string	AGRIDOME = "Agricultural Dome";
	const string	AIR_SHAFT = "Air Shaft";
	const string	CARGO_LANDER = "Cargo Lander";
	const string	CHAP = "CHAP Facility";
	const string	COLONIST_LANDER = "Colonist Lander";
	const string	COMMAND_CENTER = "Command Center";
	const string	COMMERCIAL = "Commercial";
	const string	COMM_TOWER = "Communications Tower";
	const string	FUSION_REACTOR = "Fusion Reactor";
	const string	HOT_LABORATORY = "Hot Laboratory";
	const string	LABORATORY = "Laboratory";
	const string	MEDICAL_CENTER = "Medical Center";
	const string	MINE_FACILITY = "Mine Facility";
	const string	MINE_SHAFT = "Mine Shaft";
	const string	NURSERY = "Nursery";
	const string	PARK = "Park / Reservoir ";
	const string	RECREATION_CENTER = "Recreation Center";
	const string	RED_LIGHT_DISTRICT = "Red Light District";
	const string	RESIDENCE = "Residential Facility";
	const string	ROBOT_COMMAND = "Robot Command";
	const string	SEED_FACTORY = "SEED Factory";
	const string	SEED_LANDER = "SEED Lander";
	const string	SEED_POWER = "SEED Power";
	const string	SEED_SMELTER = "SEED Smelter";
	const string	SMELTER = "Smelter";
	const string	SOLAR_PLANT = "Solar Powersat Receiver Array";
	const string	STORAGE_TANKS = "Storage Tanks";
	const string	SURFACE_FACTORY = "Surface Factory";
	const string	SURFACE_POLICE = "Police";
	const string	TUBE = "Tube";
	const string	UNDERGROUND_FACTORY = "Underground Factory";
	const string	UNDERGROUND_POLICE = "Police (UG)";
	const string	UNIVERSITY = "University";
	const string	WAREHOUSE = "Warehouse";

	// TUBE STATES
	const string	AG_TUBE_INTERSECTION = "ag_intersection";
	const string	AG_TUBE_RIGHT = "ag_right";
	const string	AG_TUBE_LEFT = "ag_left";

	const string	UG_TUBE_INTERSECTION = "ug_intersection";
	const string	UG_TUBE_RIGHT = "ug_right";
	const string	UG_TUBE_LEFT = "ug_left";

	// CURRENT LEVEL

	const string	LEVEL_SURFACE = "Surface";
	const string	LEVEL_UG1 = "Underground 1";
	const string	LEVEL_UG2 = "Underground 2";
	const string	LEVEL_UG3 = "Underground 3";
	const string	LEVEL_UG4 = "Underground 4";

	// STRUCTURE ANIMATION STATES
	const string	STRUCTURE_STATE_CONSTRUCTION = "construction";
	const string	STRUCTURE_STATE_OPERATIONAL = "operational";
	const string	STRUCTURE_STATE_OPERATIONAL_UG = "operational-ug";
	const string	STRUCTURE_STATE_DESTROYED = "destroyed";

	// PLANET SPRITE SHEET PATHS
	const string	PLANET_TYPE_MERCURY_PATH = "planets/planet_d.png";
	const string	PLANET_TYPE_MARS_PATH = "planets/planet_c.png";
	const string	PLANET_TYPE_GANYMEDE_PATH = "planets/planet_e.png";

	// UI STRINGS
	const string	WINDOW_FACTORY_PRODUCTION = "Factory Production";
	const string	WINDOW_GAME_OVER = "Game Over";
	const string	WINDOW_STRUCTURE_INSPECTOR = "Structure Inspector";
	const string	WINDOW_TILE_INSPECTOR = "Tile Inspector";
}
