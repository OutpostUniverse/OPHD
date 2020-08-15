#pragma once

#include "Structure.h"
#include "Factory.h"

class SurfaceFactory: public Factory
{
public:
	SurfaceFactory(): Factory(constants::SURFACE_FACTORY,
		"structures/factory_surface.sprite",
		StructureID::SID_SURFACE_FACTORY)
	{
		maxAge(600);
		turnsToBuild(7);
		requiresCHAP(false);

		initFactory();
	}

protected:

	void initFactory() override
	{
		addProduct(ProductType::PRODUCT_DIGGER);
		addProduct(ProductType::PRODUCT_DOZER);
		addProduct(ProductType::PRODUCT_MINER);
	}

private:
	void defineResourceInput() override
	{
		energyRequired(10);
	}
};
