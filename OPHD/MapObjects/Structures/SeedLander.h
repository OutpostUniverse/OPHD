#pragma once

#include "DeployableStructure.h"

class TileMap;
class Tile;
struct StructureBuildData;

class SeedLander : public DeployableStructure
{
public:
	SeedLander(StructureBuildData);

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			bulldoze();
			placeTubes();
			placeInitialStructures();
			deploySignal();
		}
	}

	void bulldoze() override;
	void placeTubes();
	void placeInitialStructures();

private:
	TileMap& mTileMap;
};
