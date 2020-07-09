#pragma once

#include "Structure.h"
#include <string>

const int SEED_POWER_PRODUCTION = 50;

class SeedPower: public Structure
{
public:
	SeedPower() :
		Structure(constants::SEED_POWER, "structures/seed_1.sprite", StructureClass::CLASS_ENERGY_PRODUCTION)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(150);
		turnsToBuild(5);
		requiresCHAP(false);
	}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Power Produced:";

		auto energyProduced = calculateEnergyProduced();

		stringTable[{1, 0}].text = std::to_string(energyProduced) + " / " + std::to_string(SEED_POWER_PRODUCTION);

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
		return operational() ? SEED_POWER_PRODUCTION : 0;
	}
};
