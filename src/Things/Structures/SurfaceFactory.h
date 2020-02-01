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

	virtual ~SurfaceFactory()
	{}

protected:

	virtual void initFactory()
	{
		addProduct(PRODUCT_DIGGER);
		addProduct(PRODUCT_DOZER);
		addProduct(PRODUCT_MINER);
	}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:
	virtual void defineResourceInput()
	{
		resourcesIn().energy(10);
	}
};
