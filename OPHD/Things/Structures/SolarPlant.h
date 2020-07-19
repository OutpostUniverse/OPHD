#pragma once

#include "PowerStructure.h"

#include "../../Constants.h"

const int SOLAR_PLANT_BASE_PRODUCUCTION = 2000;

class SolarPlant : public PowerStructure
{
public:
	SolarPlant() : PowerStructure(constants::SOLAR_PLANT, "structures/solar_plant.sprite", StructureClass::EnergyProduction)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(4);
		requiresCHAP(false);
	}

protected:
	void defineResourceOutput() override
	{
		resourcesOut().energy(calculateEnergyProduced());
	}

	int calculateMaxEnergyProduction() override
	{
		return SOLAR_PLANT_BASE_PRODUCUCTION;
	}
};
