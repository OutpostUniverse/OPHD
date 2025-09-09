#pragma once

#include "../Robot.h"


struct MapCoordinate;
class TileMap;
class MineFacility;


class Robominer : public Robot
{
public:
	Robominer();

	MineFacility& buildMine(TileMap& tileMap, const MapCoordinate& position);

protected:
	void onTaskComplete(TileMap& tileMap, StructureManager& structureManager) override;
};
