#pragma once

#include "Factory.h"


class SeedFactory : public Factory
{
public:
	SeedFactory() :
		Factory{
			StructureID::SID_SEED_FACTORY,
			{
				ProductType::PRODUCT_DIGGER,
				ProductType::PRODUCT_DOZER,
				ProductType::PRODUCT_MINER,
				ProductType::PRODUCT_TRUCK,
			}
		}
	{
	}
};
