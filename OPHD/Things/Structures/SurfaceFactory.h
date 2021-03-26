#pragma once

#include "Factory.h"


class SurfaceFactory: public Factory
{
public:
	SurfaceFactory(): Factory(constants::SURFACE_FACTORY,
		"structures/factory_surface.sprite",
		StructureTypeID::SID_SURFACE_FACTORY)
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
		addProduct(ProductType::PRODUCT_TRUCK);
	}

private:
	void defineResourceInput() override
	{
		energyRequired(10);
	}
};
