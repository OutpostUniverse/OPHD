#pragma once

#include "Factory.h"


class SurfaceFactory : public Factory
{
public:
	SurfaceFactory() :
		Factory{
			StructureID::SID_SURFACE_FACTORY,
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
