#pragma once

#include "Structure.h"

#include "../../Constants.h"

class SolarPanelArray : public Structure
{
public:
	SolarPanelArray() : Structure(constants::SOLAR_PANEL1, "structures/solar_array1.sprite", StructureClass::EnergyProduction)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(4);
		requiresCHAP(false);
	}

protected:
	void defineResourceOutput() override
	{
		resourcesOut().energy(25);
	}
};
