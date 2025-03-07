#pragma once

#include "Factory.h"


class UndergroundFactory : public Factory
{
public:
	UndergroundFactory() :
	Factory(
		StructureID::SID_UNDERGROUND_FACTORY,
		{
			// Need to be replaced by non robot/surface goods
			// Produces luxuries, clothing, or medicine
			ProductType::PRODUCT_CLOTHING,
			ProductType::PRODUCT_MEDICINE,
		}
	)
	{
	}
};
