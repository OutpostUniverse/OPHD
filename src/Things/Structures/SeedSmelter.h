#ifndef __SEED_SMELTER__
#define __SEED_SMELTER__

#include "Structure.h"

class SeedSmelter: public Structure
{
public:
	SeedSmelter():	Structure("Seed Smelter", "structures/seed_1.sprite")
	{
		sprite().play("construction");
		maxAge(100);
		turnsToBuild(9);
	}


	~SeedSmelter()
	{
	}


	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play("operational");
			idle(false);
			activate();
		}
	}

protected:
private:
	virtual void defineResourceInput()
	{
		mResourcesInput.energy = 5;
	}

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		mResourceValue.commonMetals = 15;
		mResourceValue.rareMetals = 3;
	}
};


#endif