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

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Power Produced:";

		auto energyProduced = calculateEnergyProduced();

		stringTable[{1, 0}].text = std::to_string(energyProduced) + " / " + std::to_string(SOLAR_PANEL_BASE_PRODUCUCTION);

		if (energyProduced == 0)
		{
			stringTable[{1, 0}].textColor = constants::WARNING_TEXT_COLOR;
		}

		return stringTable;
	}

protected:
	void defineResourceOutput() override
	{
		resourcesOut().energy(calculateEnergyProduced());
	}

private:
	int calculateEnergyProduced()
	{
		return operational() ? SOLAR_PANEL_BASE_PRODUCUCTION : 0;
	}
};
