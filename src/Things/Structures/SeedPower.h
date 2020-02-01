#pragma once

#include "Structure.h"

class SeedPower: public Structure
{
public:
	SeedPower():	Structure(constants::SEED_POWER, "structures/seed_1.sprite", CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(5);
		requiresCHAP(false);
	}


	virtual ~SeedPower()
	{}

protected:

	virtual void defineResourceInput()
	{
	}

	virtual void defineResourceOutput()
	{
		resourcesOut().energy(50);
	}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:

};
