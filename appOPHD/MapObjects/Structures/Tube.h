#pragma once

#include "../Structure.h"


class TileMap;


class Tube : public Structure
{
public:
	Tube(Tile& tile);

	void updateConnections(const TileMap& tileMap) override;
};
