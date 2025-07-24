#pragma once

#include "Factory.h"

#include <libOPHD/EnumStructureID.h>


class SurfaceFactory : public Factory
{
public:
	SurfaceFactory(Tile& tile) :
		Factory{
			StructureID::SurfaceFactory,
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
