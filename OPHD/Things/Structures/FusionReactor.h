#pragma once

#include "Structure.h"

#include "../../Constants.h"
#include <string>

const int FUSION_REACTOR_BASE_PRODUCUCTION = 1000;

class FusionReactor : public Structure
{
public:
	FusionReactor() : Structure(constants::FUSION_REACTOR, "structures/fusion_reactor.sprite", StructureClass::EnergyProduction)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1000);
		turnsToBuild(10);
		requiresCHAP(false);
	}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Power Produced:";

		auto energyProduced = calculateEnergyProduced();

		stringTable[{1, 0}].text = std::to_string(energyProduced) + " / " + std::to_string(FUSION_REACTOR_BASE_PRODUCUCTION);

		if (energyProduced == 0)
		{
			stringTable[{1, 0}].textColor = constants::WARNING_TEXT_COLOR;
		}

		return stringTable;
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().commonMetals(2);
		resourcesIn().commonMinerals(2);
		resourcesIn().rareMinerals(1);
		resourcesIn().rareMetals(1);
	}

	void defineResourceOutput() override
	{
		resourcesOut().energy(calculateEnergyProduced());
	}

private:
	int calculateEnergyProduced()
	{
		return operational() ? FUSION_REACTOR_BASE_PRODUCUCTION : 0;
	}
};
