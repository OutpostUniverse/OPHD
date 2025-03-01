#pragma once

#include "Factory.h"


class SeedFactory : public Factory
{
public:
	SeedFactory() : Factory(
		StructureID::SID_SEED_FACTORY)
	{
		addProduct(ProductType::PRODUCT_DIGGER);
		addProduct(ProductType::PRODUCT_DOZER);
		addProduct(ProductType::PRODUCT_MINER);
		addProduct(ProductType::PRODUCT_TRUCK);
	}
};
