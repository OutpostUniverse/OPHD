#pragma once

#include "Structure.h"

#include "../../Constants.h"

class SolarPlant : public Structure
{
public:
	SolarPlant() : Structure(constants::SOLAR_PLANT, "structures/solar_plant.sprite", CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(4);
		requiresCHAP(false);
	}

protected:
	void defineResourceOutput() override
	{
		resourcesOut().energy(2000);
	}
};
