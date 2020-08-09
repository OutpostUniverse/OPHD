#pragma once

#include "Structure.h"

const int StorageTanksCapacity = 1000;

class StorageTanks : public Structure
{
public:
	StorageTanks() : Structure(constants::STORAGE_TANKS, "structures/storage_tanks.sprite", StructureClass::Storage)
	{
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};
