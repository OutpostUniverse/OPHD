#pragma once

#include "PowerStructure.h"

#include "../../Common.h"
#include "../../Constants/Strings.h"


const int SOLAR_PANEL_BASE_PRODUCUCTION = 50;


class SolarPanelArray : public PowerStructure
{
public:
	SolarPanelArray() :
		PowerStructure
		{
			constants::SolarPanel1,
			"structures/solar_array1.sprite",
			StructureClass::EnergyProduction,
			StructureID::SID_SOLAR_PANEL1
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
		return static_cast<int>(SOLAR_PANEL_BASE_PRODUCUCTION / getMeanSolarDistance());
	}
};
