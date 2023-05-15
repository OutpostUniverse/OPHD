#pragma once

#include "Factory.h"

#include "../../Constants/Strings.h"


class UndergroundFactory : public Factory
{
public:
	UndergroundFactory() : Factory(
		StructureID::SID_UNDERGROUND_FACTORY)
	{
		initFactory();
	}

protected:

	void initFactory() override
	{
		// Robot digger for now. Need to be replaced by non robot/surface goods
		// Produces luxuries, clothing, or medicine
		addProduct(ProductType::PRODUCT_CLOTHING);
		addProduct(ProductType::PRODUCT_MEDICINE);
	}
};
