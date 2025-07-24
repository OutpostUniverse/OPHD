#pragma once

#include "Factory.h"

#include <libOPHD/EnumStructureID.h>


class UndergroundFactory : public Factory
{
public:
	UndergroundFactory(Tile& tile) :
		Factory{
			StructureID::UndergroundFactory,
			{
				// Need to be replaced by non robot/surface goods
				// Produces luxuries, clothing, or medicine
				ProductType::PRODUCT_CLOTHING,
				ProductType::PRODUCT_MEDICINE,
			},
			tile,
		}
	{
	}
};
