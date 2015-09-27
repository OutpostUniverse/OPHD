#ifndef __SEED_LANDER__
#define __SEED_LANDER__

#include "Structure.h"


class SeedLander: public Structure
{
public:

	typedef Gallant::Signal2<int, int> Callback;

	SeedLander(int x, int y):	Structure("Seed Lander", "structures/seed_0.sprite"),
								mX(x), mY(y)
	{
		sprite().play("construction");
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
			sprite().play("operational");
			mDeploy(mX, mY);
		}
		else if(age() == maxAge())
			sprite().play("destroyed");
	}

	Callback& deployCallback() { return mDeploy; }

protected:
private:

	Callback	mDeploy;

	int			mX, mY;
};


#endif