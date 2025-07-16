#include "ColonistLander.h"


#include "../../Map/Tile.h"


ColonistLander::ColonistLander(Tile& tile) :
	Structure{StructureClass::Lander, StructureID::SID_COLONIST_LANDER},
	mTile{tile}
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
