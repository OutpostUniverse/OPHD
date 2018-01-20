#pragma once

#include <array>
#include <memory>
#include <string>


#include <NAS2D/NAS2D.h>


enum Direction
{
	DIR_UP,
	DIR_DOWN,
	DIR_EAST,
	DIR_WEST,
	DIR_NORTH,
	DIR_SOUTH
};


enum TerrainType
{
	TERRAIN_DOZED,
	TERRAIN_CLEAR,
	TERRAIN_ROUGH,
	TERRAIN_DIFFICULT,
	TERRAIN_IMPASSABLE
};


/**
* Connector Direction.
*/
enum ConnectorDir
{
	CONNECTOR_NONE,
	CONNECTOR_INTERSECTION = 1,
	CONNECTOR_RIGHT = 2,
	CONNECTOR_LEFT = 3,
	CONNECTOR_VERTICAL			// Functions as an intersection
};


enum StructureID
{
	SID_NONE,
	SID_AGRIDOME,
	SID_AIR_SHAFT,
	SID_CHAP,
	SID_COLONIST_LANDER,
	SID_COMMAND_CENTER,
	SID_COMMERCIAL,
	SID_FUSION_REACTOR,
	SID_HOT_LABORATORY,
	SID_LABORATORY,
	SID_MEDICAL_CENTER,
	SID_MINE_FACILITY,
	SID_MINE_SHAFT,
	SID_PARK,
	SID_SURFACE_POLICE,
	SID_UNDERGROUND_POLICE,
	SID_RECREATION_CENTER,
	SID_RED_LIGHT_DISTRICT,
	SID_RESIDENCE,
	SID_ROBOT_COMMAND,
	SID_SEED_FACTORY,
	SID_SEED_LANDER,
	SID_SEED_POWER,
	SID_SEED_SMELTER,
	SID_SMELTER,
	SID_SOLAR_PLANT,
	SID_STORAGE_TANKS,
	SID_SURFACE_FACTORY,
	SID_TUBE,
	SID_UNDERGROUND_FACTORY,
	SID_UNIVERSITY,
	SID_WAREHOUSE,
	SID_NURSERY,
	SID_COMM_TOWER,
	SID_CARGO_LANDER,

	SID_COUNT
};


enum RobotType
{
	ROBOT_NONE,
	ROBOT_DIGGER,
	ROBOT_DOZER,
	ROBOT_MINER
};


enum ProductType
{
	PRODUCT_NONE = -1,

	// Surface Factories
	PRODUCT_DIGGER,
	PRODUCT_DOZER,
	PRODUCT_MINER,
	PRODUCT_EXPLORER,
	PRODUCT_TRUCK,
	PRODUCT_ROAD_MATERIALS,

	// Underground Factories
	PRODUCT_CLOTHING,
	PRODUCT_MEDICINE,
	PRODUCT_SOMETHING,
	PRODUCT_RESERVED01,
	PRODUCT_RESERVED02,
	PRODUCT_RESERVED03,
	PRODUCT_RESERVED04,
	PRODUCT_RESERVED05,
	PRODUCT_RESERVED06,
	PRODUCT_RESERVED07,
	PRODUCT_RESERVED08,
	PRODUCT_RESERVED09,
	PRODUCT_RESERVED10,

	PRODUCT_COUNT
};


/**
 * Contains population requirements for a given Structure.
 * 
 * Index 0: Workers.
 * Index 1: Scientists.
 * 
 * Any other index will result in exceptions being thrown.
 */
typedef std::array<int, 2> PopulationRequirements;


/**
 * Convenience function to pass a Rectangle_2df to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, const NAS2D::Rectangle_2df& rect);


/**
 * Convenience function to pass a \c float's to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, float rectX, float rectY, float rectW, float rectH);