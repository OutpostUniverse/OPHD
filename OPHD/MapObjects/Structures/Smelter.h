#pragma once

#include "OreRefining.h"

#include "../../Constants/Strings.h"


class Smelter : public OreRefining
{
	const int StorageCapacity = 800;

public:
	Smelter() : OreRefining(
		StructureClass::Smelter,
		StructureID::SID_SMELTER)
	{
		maxAge(600);
		turnsToBuild(9);
		requiresCHAP(false);
		hasCrime(true);
		integrityDecayRate(2);

		storageCapacity(StorageCapacity);
	}

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};
