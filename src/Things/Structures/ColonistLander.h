#pragma once

#include "Structure.h"

#include "../../Constants.h"
#include "../../Map/Tile.h"

class ColonistLander : public Structure
{
public:
	typedef NAS2D::Signals::Signal0<void> Callback;

public:

	ColonistLander(Tile* t) : Structure(constants::COLONIST_LANDER, "structures/lander_1.sprite", CLASS_LANDER), mTile(t)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(50);
		turnsToBuild(1);
		repairable(false);
		requiresCHAP(false);
		selfSustained(true);

		enable();
	}

	virtual ~ColonistLander()
	{}

	Callback& deployCallback() { return mDeploy; }

protected:

	virtual void think()
	{
		if (age() == turnsToBuild())
		{
			mDeploy();
			mTile->index(0);
		}
	}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:
	Callback	mDeploy;

	Tile*		mTile;
};
