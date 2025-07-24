#include "SeedLander.h"

#include "../../Map/Tile.h"

#include <libOPHD/EnumStructureID.h>


SeedLander::SeedLander(Tile& tile) :
	Structure{StructureID::SeedLander, tile}
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
