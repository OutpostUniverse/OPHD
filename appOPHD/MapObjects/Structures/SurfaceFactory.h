#pragma once

#include "Factory.h"

#include <libOPHD/EnumStructureID.h>


class SurfaceFactory : public Factory
{
public:
	SurfaceFactory(Tile& tile) :
		Factory{
			StructureID::SID_SURFACE_FACTORY,
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
