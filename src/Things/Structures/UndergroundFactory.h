#pragma once

#include "Structure.h"
#include "Factory.h"

class UndergroundFactory: public Factory
{
public:
	UndergroundFactory(): Factory(constants::SURFACE_FACTORY, "structures/factory_underground.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(4);

		requiresCHAP(false);

		initFactory();
	}

	virtual ~UndergroundFactory()
	{}

protected:

	virtual void initFactory()
	{
		// Robot digger for now. Need to be replaced by non robot/surface goods
		// Produces luxuries, clothing, or medicine 
		addProduct(Factory::PRODUCTION_DIGGER);
	}

private:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(10);
	}
};
