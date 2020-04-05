#pragma once

#include "Structure.h"

#include "../../ProductPool.h"

class Warehouse : public Structure
{
public:
	Warehouse() : Structure(constants::WAREHOUSE, "structures/warehouse.sprite", CLASS_WAREHOUSE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(false);
	}

	ProductPool& products() { return mProducts; }

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(1);
	}
	
private:
	ProductPool			mProducts;
};
