#include "SeedLander.h"

#include "../../Map/Tile.h"


SeedLander::SeedLander(const Tile* tile) :
	Structure{
		StructureClass::Lander,
		StructureID::SID_SEED_LANDER
	},
	mPosition{tile->xy()}
{
	enable();
}


SeedLander::Signal::Source& SeedLander::deploySignal()
{
	return mDeploy;
}


void SeedLander::think()
{
	if (age() == turnsToBuild())
	{
		mDeploy(mPosition);
	}
}
