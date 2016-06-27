#pragma once

#include "Structure.h"


class SeedLander: public Structure
{
public:

	typedef Gallant::Signal2<int, int> Callback;

	SeedLander(int x, int y):	Structure(constants::SEED_LANDER, "structures/seed_0.sprite", STRUCTURE_LANDER),
								mX(x), mY(y)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(50);
		turnsToBuild(1);
		repairable(false);
		requiresCHAP(false);
		selfSustained(true);

		enable();

		defineResourceCostToBuild();
	}


	~SeedLander()
	{}

	Callback& deployCallback() { return mDeploy; }

protected:

	virtual void think()
	{
		if (age() == turnsToBuild())
			mDeploy(mX, mY);
	}

	// Counterintuitive because these structures can't be build directly but
	// basically this is defined so we can get a recycling value.
	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(10);
		resourcesCostToBuild().rareMetals(5);
		resourcesCostToBuild().commonMinerals(10);
		resourcesCostToBuild().rareMinerals(5);
	}

private:

	Callback	mDeploy;

	int			mX, mY;
};
