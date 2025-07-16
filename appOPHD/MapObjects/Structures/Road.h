#pragma once

#include "../Structure.h"


class TileMap;


class Road : public Structure
{
public:
	Road(Tile& tile) :
		Structure{StructureClass::Road, StructureID::SID_ROAD, tile}
	{
	}


	void updateConnections(const TileMap& tileMap);
};
