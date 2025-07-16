#pragma once

#include "../Structure.h"


class TileMap;


class Road : public Structure
{
public:
	Road() :
		Structure(StructureClass::Road, StructureID::SID_ROAD)
	{
	}


	void updateConnections(const TileMap& tileMap);
};
