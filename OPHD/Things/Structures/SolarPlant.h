#pragma once

#include "PowerStructure.h"

#include "../../Common.h"
#include "../../Constants/Strings.h"


const int SOLAR_PLANT_BASE_PRODUCUCTION = 2000;


class SolarPlant : public PowerStructure
{
public:
	SolarPlant() :
		PowerStructure
		{
			constants::SolarPlant,
			"structures/solar_plant.sprite",
			StructureClass::EnergyProduction,
			StructureID::SID_SOLAR_PLANT
		}
	{
		maxAge(1000);
		turnsToBuild(4);
		requiresCHAP(false);
		hasCrime(true);
	}

protected:
	int calculateMaxEnergyProduction() override
	{
		return static_cast<int>(SOLAR_PLANT_BASE_PRODUCUCTION / getMeanSolarDistance());
	}
};
