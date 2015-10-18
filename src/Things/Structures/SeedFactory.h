#ifndef __SEED_FACTORY__
#define __SEED_FACTORY__

#include "Structure.h"
#include "Factory.h"

class SeedFactory: public Factory
{
public:
	SeedFactory(): Factory(constants::SEED_FACTORY, "structures/seed_1.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(100);
		turnsToBuild(7);
		requiresCHAP(false);
		isFactory(true);
	}

	~SeedFactory()
	{
	}

	void update()
	{
		incrementAge();

		if (age() == turnsToBuild())
		{
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
			idle(false);
			activate();
		}
		else if (age() == maxAge())
		{
			sprite().play(constants::STRUCTURE_STATE_DESTROYED);
		}
	}

protected:

	virtual void initFactory()
	{
		// Realy this should be dependant on what's being produced.
		productionLength(5);
	}

	virtual void productionComplete()
	{
	}

private:
	virtual void defineResourceInput()
	{
		mResourcesInput.energy = 15;
	}
};


#endif