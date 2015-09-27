#ifndef __OPHD_COMMON__
#define __OPHD_COMMON__



enum Direction
{
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
	CONNECTOR_INTERSECTION,
	CONNECTOR_RIGHT,
	CONNECTOR_LEFT,
	CONNECTOR_VERTICAL
};

#endif