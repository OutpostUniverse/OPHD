#pragma once

#include "Structure.h"
#include "Factory.h"

class UndergroundFactory: public Factory
{
public:
	UndergroundFactory(): Factory(constants::UNDERGROUND_FACTORY, "structures/factory_underground.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(4);

		requiresCHAP(false);

		initFactory();
	}

protected:

	void initFactory() override
	{
		// Robot digger for now. Need to be replaced by non robot/surface goods
		// Produces luxuries, clothing, or medicine 
		addProduct(ProductType::PRODUCT_CLOTHING);
		addProduct(ProductType::PRODUCT_MEDICINE);
	}

private:
	void defineResourceInput() override
	{
		resourcesIn().energy(10);
	}
};
