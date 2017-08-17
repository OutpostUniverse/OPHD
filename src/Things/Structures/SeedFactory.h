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
		turnsToBuild(8);
		requiresCHAP(false);

		initFactory();
	}

	virtual ~SeedFactory()
	{}

protected:

	virtual void initFactory()
	{
		addProduct(PRODUCT_DIGGER);
		addProduct(PRODUCT_DOZER);
		addProduct(PRODUCT_MINER);
	}

private:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}
};
