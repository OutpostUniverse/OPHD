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

		defineResourceCostToBuild();
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

	// Counterintuitive because these structures can't be build directly but
	// basically this is defined so we can get a recycling value.
	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(15);
		resourcesCostToBuild().rareMetals(10);
		resourcesCostToBuild().commonMinerals(10);
		resourcesCostToBuild().rareMinerals(8);
	}

private:

};
