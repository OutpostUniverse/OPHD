#pragma once

#include "PowerStructure.h"

#include "../../Constants.h"

const int SOLAR_PANEL_BASE_PRODUCUCTION = 25;

class SolarPanelArray : public PowerStructure
{
public:
	SolarPanelArray() : PowerStructure(constants::SOLAR_PANEL1, "structures/solar_array1.sprite", StructureClass::CLASS_ENERGY_PRODUCTION)
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

private:
	int calculateEnergyProduced() override
	{
		return operational() ? SOLAR_PANEL_BASE_PRODUCUCTION : 0;
	}

	int calculateMaxEnergyProduction() override
	{
		return SOLAR_PANEL_BASE_PRODUCUCTION;
	}
};
