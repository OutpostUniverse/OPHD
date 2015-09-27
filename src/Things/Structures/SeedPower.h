#ifndef __SEED_POWER__
#define __SEED_POWER__

#include "Structure.h"

class SeedPower: public Structure
{
public:
	SeedPower():	Structure("Seed Power", "structures/seed_1.sprite")
	{
		sprite().play("construction");
		maxAge(100);
		turnsToBuild(3);
	}


	~SeedPower()
	{}


	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play("operational");
			idle(false);
			activate();
		}
		else if(age() == maxAge())
			sprite().play("destroyed");
	}

protected:

	virtual void defineResourceInput()
	{}

	virtual void defineResourceOutput()
	{
		mResourcesOutput.energy = 50;
	}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		mResourceValue.commonMetals = 20;
		mResourceValue.rareMetals = 5;
	}

private:

};


#endif