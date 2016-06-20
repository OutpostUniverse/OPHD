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

protected:

	virtual void think()
	{
		/** \todo	At the moment we're only adding one food unit per turn. In
		*			the future we'll want to add modifiers based on research,
		*			difficulty and hostility level of the planet.
		*/
		if (storage().atCapacity())
		{
			idle();
		}
		else
		{
			if (storage().pushResource(ResourcePool::RESOURCE_FOOD, 1) != 0)
				idle();
		}

	}

	virtual void defineResourceInput()
	{
		resourcesIn().commonMinerals(2);
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		resourcesValue().commonMetals(20);
		resourcesValue().rareMetals(5);
	}

private:

};


