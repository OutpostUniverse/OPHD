#include "CargoLander.h"

#include "../../Map/Tile.h"

#include <libOPHD/EnumStructureID.h>


CargoLander::CargoLander(Tile& tile) :
	Structure{StructureID::CargoLander, tile}
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
		mTile.bulldoze();
	}
}
