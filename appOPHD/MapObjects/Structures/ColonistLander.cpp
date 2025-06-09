#include "ColonistLander.h"


#include "../../Map/Tile.h"


ColonistLander::ColonistLander(Tile* tile) : Structure(
	StructureClass::Lander,
	StructureID::SID_COLONIST_LANDER),
	mTile(tile)
{
	enable();
}


ColonistLander::Signal::Source& ColonistLander::deploySignal() { return mDeploy; }


void ColonistLander::think()
{
	if (age() == turnsToBuild())
	{
		mDeploy();
		mTile->index(TerrainType::Dozed);
	}
}
