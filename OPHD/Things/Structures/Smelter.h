#pragma once

#include "OreRefining.h"

class Smelter : public OreRefining
{
	const int StorageCapacity = 800;

public:
	Smelter() : OreRefining(constants::SMELTER,
		"structures/smelter.sprite",
		StructureClass::Smelter,
		StructureTypeID::SID_SMELTER)
	{
		maxAge(600);
		turnsToBuild(9);
		requiresCHAP(false);

		storageCapacity(StorageCapacity);
	}

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};
