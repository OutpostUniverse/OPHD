#pragma once

#include "Structure.h"

class SeedPower: public Structure
{
public:
	SeedPower():	Structure(constants::SEED_POWER, "structures/seed_1.sprite", STRUCTURE_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(100);
		turnsToBuild(5);
		requiresCHAP(false);
	}


	~SeedPower()
	{}

protected:

	virtual void defineResourceInput()
	{
	}

	virtual void defineResourceOutput()
	{
		resourcesOut().energy(50);
	}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		resourcesValue().commonMetals(20);
		resourcesValue().rareMetals(5);
	}

private:

};
