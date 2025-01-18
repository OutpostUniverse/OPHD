#pragma once

#include "DeployableStructure.h"

#include "OPHD/Map/Tile.h"


class SeedLander : public DeployableStructure
{
public:
using SeedLanderSignal = NAS2D::Signal<NAS2D::Point<int>>;

public:
	SeedLander(Tile& tile) : DeployableStructure(
		StructureClass::Lander,
		StructureID::SID_SEED_LANDER,
		tile)
	{}

	SeedLanderSignal::Source& seedLanderDeploySignal() { return mSeedLanderDeploySignal; }

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			mSeedLanderDeploySignal(point());
		}
	}

private:
	SeedLanderSignal mSeedLanderDeploySignal;
	NAS2D::Point<int> point() { return mTile.xy(); }
};
