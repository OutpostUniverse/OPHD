#pragma once

#include "Structure.h"

#include "../../Constants.h"

const int SOLAR_PANEL_BASE_PRODUCUCTION = 25;

class SolarPanelArray : public Structure
{
public:
	SolarPanelArray() : Structure(constants::SOLAR_PANEL1, "structures/solar_array1.sprite", StructureClass::CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(4);
		requiresCHAP(false);
	}

protected:
	void defineResourceOutput() override
	{
		resourcesOut().energy(SOLAR_PANEL_BASE_PRODUCUCTION);
	}
};
