#pragma once

#include "Factory.h"

#include "../../Constants/Strings.h"


class SurfaceFactory : public Factory
{
public:
	SurfaceFactory() : Factory(
		StructureID::SID_SURFACE_FACTORY)
	{
		maxAge(600);
		turnsToBuild(7);
		requiresCHAP(false);
		hasCrime(true);

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
