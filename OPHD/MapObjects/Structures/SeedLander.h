#pragma once

#include "DeployableStructure.h"

#include "OPHD/Map/Tile.h"

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
	NAS2D::Point<int> point() { return mTile.xy(); }
	TileMap& mTileMap;
};
