#pragma once

#include "OreRefining.h"


class SeedSmelter : public OreRefining
{
	const int StorageCapacity = 500;

public:
	SeedSmelter() : OreRefining(constants::SEED_SMELTER, "structures/seed_1.sprite", StructureClass::Smelter)
	{
		maxAge(150);
		turnsToBuild(6);
		requiresCHAP(false);
	}

protected:
	int calculateMaxStorage() override
	{
		return StorageCapacity;
	}

private:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};
