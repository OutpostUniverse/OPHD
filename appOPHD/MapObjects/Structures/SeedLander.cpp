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

void SeedLander::position(NAS2D::Point<int> position)
{
	mPosition = position;
}


SeedLander::Signal::Source& SeedLander::deploySignal()
{
	return mDeploy;
}


void SeedLander::think()
{
	if (age() == turnsToBuild())
	{
		// Logic guard, probably not necessary.
		if (mPosition == NAS2D::Point{0, 0}) { return; }

		mDeploy(mPosition);
	}
}
