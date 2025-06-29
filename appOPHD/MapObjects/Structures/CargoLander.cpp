#include "CargoLander.h"


#include "../../Map/Tile.h"


CargoLander::CargoLander(Tile* tile) : Structure(
	StructureClass::Lander,
	StructureID::SID_CARGO_LANDER),
	mTile(tile)
{
	enable();
}


void CargoLander::deployHandler(DeployDelegate newDeployHandler)
{
	if (newDeployHandler) { mDeploy.connect(newDeployHandler); }
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
