#ifndef __OPHD_STRINGS__
#define __OPHD_STRINGS__


#include <iostream>

using namespace std;


namespace constants
{
	// ROBOTS
	const string	ROBODIGGER = "Robodigger";
	const string	ROBODOZER = "Robodozer";
	const string	ROBOMINER = "Robominer";


	// STRUCTURES
	const string	AGRIDOME = "Agricultural Dome";
	const string	AIR_SHAFT = "Air Shaft";
	const string	CHAP = "CHAP Facility";
	const string	COMMAND_CENTER = "Command Center";
	const string	MINE_FACILITY = "Mine Facility";
	const string	SEED_FACTORY = "SEED Factory";
	const string	SEED_LANDER = "SEED Lander";
	const string	SEED_POWER = "SEED Power";
	const string	SEED_SMELTER = "SEED Smelter";
	const string	TUBE = "Tube";

	// TUBE STATES
	const string	AG_TUBE_INTERSECTION = "ag_intersection";
	const string	AG_TUBE_RIGHT = "ag_right";
	const string	AG_TUBE_LEFT = "ag_left";

	const string	UG_TUBE_INTERSECTION = "ug_intersection";
	const string	UG_TUBE_RIGHT = "ug_right";
	const string	UG_TUBE_LEFT = "ug_left";

	// STRUCTURE ANIMATION STATES
	const string	STRUCTURE_STATE_CONSTRUCTION = "construction";
	const string	STRUCTURE_STATE_OPERATIONAL = "operational";
	const string	STRUCTURE_STATE_OPERATIONAL_UG = "operational-ug";
	const string	STRUCTURE_STATE_DESTROYED = "destroyed";
}

#endif