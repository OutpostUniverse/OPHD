#include "ColonistLander.h"

#include "../../Map/Tile.h"

#include <libOPHD/EnumStructureID.h>


ColonistLander::ColonistLander(Tile& tile) :
	Structure{StructureID::ColonistLander, tile}
{
	enable();
}


void ColonistLander::deployHandler(DeployDelegate newDeployHandler)
{
	mDeployHandler = newDeployHandler;
}


void ColonistLander::think()
{
	if (age() == turnsToBuild())
	{
		if (mDeployHandler) { mDeployHandler(); }
		mTile.bulldoze();
	}
}
