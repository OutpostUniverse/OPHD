#pragma once

#include "PowerStructure.h"

#include "../../Constants.h"

const int SOLAR_PANEL_BASE_PRODUCUCTION = 50;

class SolarPanelArray : public PowerStructure
{
public:
	SolarPanelArray(float meanSolarDistance) : PowerStructure(constants::SOLAR_PANEL1, "structures/solar_array1.sprite", StructureClass::EnergyProduction), 
		mMeanSolarDistance(meanSolarDistance)
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
		return static_cast<int>(SOLAR_PANEL_BASE_PRODUCUCTION / mMeanSolarDistance);
	}

private:
	const float mMeanSolarDistance;
};
