#include "SeedLander.h"

#include "../../Map/Tile.h"


SeedLander::SeedLander(Tile& tile) :
	Structure{StructureID::SID_SEED_LANDER, tile}
{
	enable();
}


void SeedLander::deployHandler(DeployDelegate newDeployHandler)
{
	mDeployHandler = newDeployHandler;
}


void SeedLander::think()
{
	if (age() == turnsToBuild())
	{
		if (mDeployHandler) { mDeployHandler(mTile.xy()); }
	}
}
