#pragma once

#include "Structure.h"

class Agridome : public Structure
{
public:
	Agridome() : Structure(constants::AGRIDOME, "structures/agridome.sprite", STRUCTURE_FOOD_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(5);

		storage().capacity(1000);
	}


	~Agridome()
	{}


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
			enabled(false);
			sprite().play(constants::STRUCTURE_STATE_DESTROYED);
		}

		/** \todo	At the moment we're only adding one food unit per turn. In
		 *			the future we'll want to add modifiers based on research,
		 *			difficulty and hostility level of the planet.
		 */
		if (storage().atCapacity())
		{
			idle(true);
		}
		else
		{
			if (storage().pushResource(ResourcePool::RESOURCE_FOOD, 1) != 0)
				idle(true);
		}
	}

protected:

	virtual void defineResourceInput()
	{
		mResourcesInput.commonMinerals(2);
		mResourcesInput.energy(1);
	}

	virtual void defineResourceOutput()
	{}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		mResourceValue.commonMetals(20);
		mResourceValue.rareMetals(5);
	}

private:

};


