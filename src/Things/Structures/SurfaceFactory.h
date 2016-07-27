#pragma once

#include "Structure.h"
#include "Factory.h"

class SurfaceFactory: public Factory
{
public:
	SurfaceFactory(): Factory(constants::SURFACE_FACTORY, "structures/factory_surface.sprite")
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(7);
		requiresCHAP(false);

		initFactory();
	}

	~SurfaceFactory()
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
		resourcesIn().energy(10);
	}
};
