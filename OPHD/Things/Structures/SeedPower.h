#pragma once

#include "Structure.h"

const int SEED_POWER_PRODUCTION = 50;

class SeedPower: public Structure
{
public:
	SeedPower() :
		Structure(constants::SEED_POWER, "structures/seed_1.sprite", StructureClass::CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(5);
		requiresCHAP(false);
	}

protected:
	void defineResourceOutput() override
	{
		resourcesOut().energy(SEED_POWER_PRODUCTION);
	}
};
