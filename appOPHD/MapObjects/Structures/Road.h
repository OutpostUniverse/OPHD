#pragma once

#include "../Structure.h"

#include <libOPHD/EnumStructureID.h>


class TileMap;


class Road : public Structure
{
public:
	Road(Tile& tile) :
		Structure{StructureID::SID_ROAD, tile}
	{
	}


	void updateConnections(const TileMap& tileMap);
};
