#pragma once

#include "Structure.h"

#include "../../Constants.h"


const int StorageTanksCapacity = 1000;


class StorageTanks : public Structure
{
public:
	StorageTanks() : Structure(constants::STORAGE_TANKS,
		"structures/storage_tanks.sprite",
		StructureClass::Storage,
		StructureID::SID_STORAGE_TANKS)
	{
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(false);
		storageCapacity(StorageTanksCapacity);
		hasCrime(true);
	}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Storage Capacity:";
		stringTable[{1, 0}].text = std::to_string(storageCapacity());

		return stringTable;
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};
