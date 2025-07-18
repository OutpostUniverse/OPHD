#pragma once

#include "PowerStructure.h"


const int SeedPowerProduction = 50;

class SeedPower : public PowerStructure
{
public:
	SeedPower(Tile& tile) :
		PowerStructure{StructureID::SID_SEED_POWER, tile}
	{
	}

private:
	int calculateMaxEnergyProduction() const override
	{
		return SeedPowerProduction;
	}
};
