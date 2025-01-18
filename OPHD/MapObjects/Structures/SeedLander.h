#pragma once

#include "DeployableStructure.h"

#include "OPHD/Map/Tile.h"

class TileMap;
class Tile;

class SeedLander : public DeployableStructure
{
public:
using SeedLanderSignal = NAS2D::Signal<NAS2D::Point<int>>;

public:
	SeedLander(Tile& tile, TileMap& tileMap);

	SeedLanderSignal::Source& seedLanderDeploySignal() { return mSeedLanderDeploySignal; }

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			bulldoze();
			mSeedLanderDeploySignal(point());
		}
	}

	void bulldoze() override;

private:
	SeedLanderSignal mSeedLanderDeploySignal;
	NAS2D::Point<int> point() { return mTile.xy(); }
	TileMap& mTileMap;
};
