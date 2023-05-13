#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"
#include "../../Map/Tile.h"


class ColonistLander : public Structure
{
public:
	using Signal = NAS2D::Signal<>;

public:

	ColonistLander(Tile* tile) : Structure(
		StructureClass::Lander,
		StructureID::SID_COLONIST_LANDER),
		mTile(tile)
	{
		maxAge(50);
		turnsToBuild(1);
		repairable(false);
		requiresCHAP(false);
		selfSustained(true);
		integrityDecayRate(2);

		enable();
	}

	Signal::Source& deploySignal() { return mDeploy; }

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			mDeploy();
			mTile->index(TerrainType::Dozed);
		}
	}

private:
	Signal mDeploy;

	Tile* mTile;
};
