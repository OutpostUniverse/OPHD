#pragma once

#include "Structure.h"
#include "../../Constants.h"
#include <string>

const int SEED_POWER_PRODUCTION = 50;

class SeedPower: public PowerStructure
{
public:
	SeedPower() : PowerStructure(constants::SEED_POWER, "structures/seed_1.sprite", StructureClass::EnergyProduction)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(5);
		requiresCHAP(false);
	}

protected:
	void defineResourceOutput() override
	{
		resourcesOut().energy(calculateEnergyProduced());
	}

private:
	int calculateMaxEnergyProduction() override
	{
		return SEED_POWER_PRODUCTION;
	}
};
