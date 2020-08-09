#pragma once

#include "Structure.h"

#include "../../ProductPool.h"

class Warehouse : public Structure
{
public:
	Warehouse() : Structure(constants::WAREHOUSE, "structures/warehouse.sprite", StructureClass::Warehouse)
	{
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(false);
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
