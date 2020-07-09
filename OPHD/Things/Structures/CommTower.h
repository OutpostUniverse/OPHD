#pragma once

#include "Structure.h"

#include "../../Constants.h"
#include <sstream>

class CommTower : public Structure
{
public:

	CommTower() : Structure(constants::COMM_TOWER, "structures/communications_tower.sprite", StructureClass::CLASS_COMM)
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

		std::stringstream stringStream;
		stringStream.precision(0);
		stringStream << communicationRange;

		stringTable[{1, 0}].text = stringStream.str();;

		if (communicationRange == 0)
		{
			stringTable[{1, 0}].textColor = constants::WARNING_TEXT_COLOR;
		}

		return stringTable;
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn().energy(3);
	}
};
