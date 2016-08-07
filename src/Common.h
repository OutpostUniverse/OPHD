#pragma once

#include <memory>
#include <string>

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
	SID_COMMAND_CENTER,
	SID_FUSION_REACTOR,
	SID_MINE_FACILITY,
	SID_MINE_SHAFT,
	SID_PARK,
	SID_SURFACE_POLICE,
	SID_UNDERGROUND_POLICE,
	SID_RECREATION_CENTER,
	SID_RESIDENCE,
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
};


enum RobotType
{
	ROBOT_NONE,
	ROBOT_DIGGER,
	ROBOT_DOZER,
	ROBOT_MINER
};


/**
 * Simple helper function to provide a printf like function.
 */
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	unique_ptr<char[]> buffer(new char[size]);
	snprintf(buffer.get(), size, format.c_str(), args ...);
	return string(buffer.get(), buffer.get() + size - 1);
}