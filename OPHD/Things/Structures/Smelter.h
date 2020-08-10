#pragma once

#include "OreRefining.h"

class Smelter : public OreRefining
{
	const int StorageCapacity = 800;

public:
	Smelter() : OreRefining(constants::SMELTER, "structures/smelter.sprite", StructureClass::Smelter)
	{
		maxAge(600);
		turnsToBuild(9);
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
