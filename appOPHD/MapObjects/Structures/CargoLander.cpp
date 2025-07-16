#include "CargoLander.h"


#include "../../Map/Tile.h"


CargoLander::CargoLander(Tile& tile) :
	Structure{StructureClass::Lander, StructureID::SID_CARGO_LANDER},
	mTile{&tile}
{
	enable();
}


void CargoLander::deployHandler(DeployDelegate newDeployHandler)
{
	mDeployHandler = newDeployHandler;
}


void CargoLander::think()
{
	if (age() == turnsToBuild())
	{
		if (mDeployHandler) { mDeployHandler(); }
		mTile->bulldoze();
	}
}
