#pragma once

#include "Structure.h"

class Warehouse : public Structure
{
public:
	Warehouse() : Structure(constants::WAREHOUSE, "structures/warehouse.sprite", STRUCTURE_WAREHOUSE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(false);

		// for now general capacity
		// Need to be converted in specific capacity for robots and luxury goods
		storage().capacity(1000);
	}


	~Warehouse()
	{}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

private:

};
