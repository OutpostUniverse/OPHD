#pragma once

#include "Structure.h"

#include "../../Constants.h"
#include "../../Map/Tile.h"

class ColonistLander : public Structure
{
public:
	using Callback = NAS2D::Signals::Signal<>;

public:

	ColonistLander(Tile* t) : Structure(constants::COLONIST_LANDER,
		"structures/lander_1.sprite",
		StructureClass::Lander,
		StructureID::SID_COLONIST_LANDER),
		mTile(t)
	{
		maxAge(50);
		turnsToBuild(1);
		repairable(false);
		requiresCHAP(false);
		selfSustained(true);

		enable();
	}

	Callback& deployCallback() { return mDeploy; }

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			mDeploy();
			mTile->index(TerrainType::TERRAIN_DOZED);
		}
	}

private:
	Callback mDeploy;

	Tile* mTile;
};
