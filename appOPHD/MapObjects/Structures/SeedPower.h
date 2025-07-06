#pragma once

#include "PowerStructure.h"


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
	int calculateMaxEnergyProduction() const override
	{
		return SeedPowerProduction;
	}
};
