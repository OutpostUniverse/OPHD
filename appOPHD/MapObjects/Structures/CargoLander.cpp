#include "CargoLander.h"


#include "../../Map/Tile.h"


CargoLander::CargoLander(Tile* tile) : Structure(
	StructureClass::Lander,
	StructureID::SID_CARGO_LANDER),
	mTile(tile)
{
	enable();
}


CargoLander::Signal::Source& CargoLander::deploySignal()
{
	return mDeploy;
}


void CargoLander::think()
{
	if (age() == turnsToBuild())
	{
		mDeploy();
		mTile->bulldoze();
	}
}
