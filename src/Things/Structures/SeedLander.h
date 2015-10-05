#ifndef __SEED_LANDER__
#define __SEED_LANDER__

#include "Structure.h"


class SeedLander: public Structure
{
public:

	typedef Gallant::Signal2<int, int> Callback;

	SeedLander(int x, int y):	Structure(constants::SEED_LANDER, "structures/seed_0.sprite"),
								mX(x), mY(y)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(50);
		turnsToBuild(1);
		repairable(false);

		// Resource value if dmolished.
		mResourceValue.commonMetals = 10;
		mResourceValue.rareMetals = 2;
	}


	~SeedLander()
	{
	}

	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
			mDeploy(mX, mY);
		}
		else if(age() == maxAge())
			sprite().play(constants::STRUCTURE_STATE_DESTROYED);
	}

	Callback& deployCallback() { return mDeploy; }

protected:
private:

	Callback	mDeploy;

	int			mX, mY;
};


#endif