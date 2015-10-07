#ifndef __SEED_POWER__
#define __SEED_POWER__

#include "Structure.h"

class SeedPower: public Structure
{
public:
	SeedPower():	Structure(constants::SEED_POWER, "structures/seed_1.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(100);
		turnsToBuild(3);
		requiresCHAP(false);
	}


	~SeedPower()
	{}


	void update()
	{
		incrementAge();

		if(age() == turnsToBuild())
		{
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
			idle(false);
			activate();
		}
		else if(age() == maxAge())
			sprite().play(constants::STRUCTURE_STATE_DESTROYED);
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