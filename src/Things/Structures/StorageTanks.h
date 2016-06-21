#pragma once

#include "Structure.h"

class StorageTanks : public Structure
{
public:
	StorageTanks() : Structure(constants::STORAGE_TANKS, "structures/storage_tanks.sprite", STRUCTURE_STORAGE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		storage().capacity(1000);

		defineResourceCostToBuild();
	}


	~StorageTanks()
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

	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(15);
		resourcesCostToBuild().rareMetals(6);
		resourcesCostToBuild().commonMinerals(5);
		resourcesCostToBuild().rareMinerals(1);
	}

private:

};
