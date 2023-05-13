#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"
#include "../../ProductPool.h"


class Warehouse : public Structure
{
public:
	Warehouse() : Structure(
		"structures/warehouse.sprite",
		StructureClass::Warehouse,
		StructureID::SID_WAREHOUSE)
	{
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(false);
		hasCrime(true);
	}

	ProductPool& products() { return mProducts; }

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}

private:
	ProductPool mProducts;
};
