#pragma once

#include "PowerStructure.h"

#include <string>


const int SeedPowerProduction = 50;

class SeedPower : public PowerStructure
{
public:
	SeedPower() : PowerStructure(
		StructureClass::EnergyProduction,
		StructureID::SID_SEED_POWER)
	{
	}

private:
	int calculateMaxEnergyProduction() override
	{
		return SeedPowerProduction;
	}
};
