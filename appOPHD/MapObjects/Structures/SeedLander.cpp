#include "SeedLander.h"

#include "../../Map/Tile.h"


SeedLander::SeedLander(const Tile* tile) :
	Structure{StructureClass::Lander, StructureID::SID_SEED_LANDER},
	mPosition{tile->xy()}
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
		if (mDeployHandler) { mDeployHandler(mPosition); }
	}
}
