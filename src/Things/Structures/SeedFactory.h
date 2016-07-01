#pragma once

#include "Structure.h"
#include "Factory.h"

class SeedFactory: public Factory
{
public:
	SeedFactory(): Factory(constants::SEED_FACTORY, "structures/seed_1.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(7);
		requiresCHAP(false);

		initFactory();
		defineResourceCostToBuild();
	}

	~SeedFactory()
	{}

protected:

	virtual void initFactory()
	{
		addProduct(Factory::PRODUCTION_DIGGER);
		addProduct(Factory::PRODUCTION_DOZER);
		addProduct(Factory::PRODUCTION_MINER);
	}

private:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}

	// Counterintuitive because these structures can't be build directly but
	// basically this is defined so we can get a recycling value.
	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(20);
		resourcesCostToBuild().rareMetals(10);
		resourcesCostToBuild().commonMinerals(10);
		resourcesCostToBuild().rareMinerals(5);
	}
};
