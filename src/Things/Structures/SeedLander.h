#ifndef __SEED_LANDER__
#define __SEED_LANDER__

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

	virtual void defineResourceValue()
	{
		resourcesValue().commonMetals(10);
		resourcesValue().rareMetals(2);
	}

private:

	Callback	mDeploy;

	int			mX, mY;
};


#endif