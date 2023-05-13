#pragma once

#include "Factory.h"

#include "../../Constants/Strings.h"


class SeedFactory : public Factory
{
public:
	SeedFactory() : Factory(
		"structures/seed_1.sprite",
		StructureID::SID_SEED_FACTORY)
	{
		maxAge(150);
		turnsToBuild(8);
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
		energyRequired(5);
	}
};
