#pragma once

#include "PowerStructure.h"

#include "../../Constants.h"

#include <string>


const int SEED_POWER_PRODUCTION = 50;

class SeedPower : public PowerStructure
{
public:
	SeedPower() : PowerStructure(constants::SeedPower,
		"structures/seed_1.sprite",
		StructureClass::EnergyProduction,
		StructureID::SID_SEED_POWER)
	{
		maxAge(150);
		turnsToBuild(5);
		requiresCHAP(false);
	}

private:
	int calculateMaxEnergyProduction() override
	{
		return SEED_POWER_PRODUCTION;
	}
};
