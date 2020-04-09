#pragma once

#include "Structure.h"

#include "../../Constants.h"
#include "../../Map/Tile.h"

class ColonistLander : public Structure
{
public:
	typedef NAS2D::Signals::Signal<> Callback;

public:

	ColonistLander(Tile* t) : Structure(constants::COLONIST_LANDER, "structures/lander_1.sprite", StructureClass::CLASS_LANDER), mTile(t)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
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
			mTile->index(0);
		}
	}

private:
	Callback	mDeploy;

	Tile*		mTile;
};
