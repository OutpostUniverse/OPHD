#pragma once

#include "Deployable.h"
#include "OPHD/MapObjects/Structure.h"
#include "OPHD/Map/Tile.h"

#include <NAS2D/Signal/Signal.h>
#include <functional>

class DeployableStructure : public Deployable, public Structure
{
public:
	DeployableStructure(StructureClass structureClass, StructureID id, Tile& tile) :
	Structure(structureClass, id),
	mTile(tile)
	{
		enable();
	}

	SignalSource& deploySignal() override { return mDeploySignal; }

protected:
	Signal mDeploySignal;
	Tile& mTile;

protected:
	void bulldoze() override { mTile.index(TerrainType::Dozed); }
	void think() override
	{
		if (age() == turnsToBuild())
		{
			bulldoze();
			mDeploySignal();
		}
	}
};
