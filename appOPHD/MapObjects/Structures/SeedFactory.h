#pragma once

#include "Factory.h"

#include <libOPHD/EnumStructureID.h>


class SeedFactory : public Factory
{
public:
	SeedFactory(Tile& tile) :
		Factory{
			StructureID::SID_SEED_FACTORY,
			{
				ProductType::PRODUCT_DIGGER,
				ProductType::PRODUCT_DOZER,
				ProductType::PRODUCT_MINER,
				ProductType::PRODUCT_TRUCK,
			},
			tile,
		}
	{
	}
};
