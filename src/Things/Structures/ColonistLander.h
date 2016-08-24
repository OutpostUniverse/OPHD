#pragma once

#include "Structure.h"

#include "../../Constants.h"

class ColonistLander : public Structure
{
public:

	typedef Gallant::Signal0<void> Callback;

	ColonistLander() : Structure(constants::SEED_LANDER, "structures/lander_1.sprite", STRUCTURE_LANDER)
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
		}
	}

private:

	Callback	mDeploy;
};
