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

protected:

	void initFactory() override
	{
		addProduct(PRODUCT_DIGGER);
		addProduct(PRODUCT_DOZER);
		addProduct(PRODUCT_MINER);
		addProduct(PRODUCT_ROAD_MATERIALS);
	}

private:

	void defineResourceInput() override
	{
		resourcesIn().energy(5);
	}
};
