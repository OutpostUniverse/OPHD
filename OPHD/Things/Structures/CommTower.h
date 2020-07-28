#pragma once

#include "Structure.h"

#include "../../Constants.h"


class CommTower : public Structure
{
public:

	CommTower() : Structure(constants::COMM_TOWER, "structures/communications_tower.sprite", StructureClass::Communication)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(600);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
	}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Communication Range:";

		auto communicationRange = operational() ? constants::COMM_TOWER_BASE_RANGE : 0;
		stringTable[{1, 0}].text = std::to_string(communicationRange);

		if (communicationRange == 0)
		{
			stringTable[{1, 0}].textColor = constants::WARNING_TEXT_COLOR;
		}

		return stringTable;
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(3);
	}
};
