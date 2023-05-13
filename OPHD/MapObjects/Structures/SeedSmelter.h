#pragma once

#include "OreRefining.h"

#include "../../Constants/Strings.h"


class SeedSmelter : public OreRefining
{
	const int StorageCapacity = 500;

public:
	SeedSmelter() : OreRefining(
		"structures/seed_1.sprite",
		StructureClass::Smelter,
		StructureID::SID_SEED_SMELTER)
	{
		maxAge(150);
		turnsToBuild(6);
		requiresCHAP(false);

		storageCapacity(StorageCapacity);
	}

protected:

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};
