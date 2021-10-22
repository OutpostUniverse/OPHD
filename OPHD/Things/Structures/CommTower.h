#pragma once

#include "Structure.h"

#include "../../Constants.h"
#include "../../Constants/UiConstants.h"


class CommTower : public Structure
{
private:
	const int BaseRange = 10;

public:
	CommTower() : Structure(constants::CommTower,
		"structures/communications_tower.sprite",
		StructureClass::Communication,
		StructureID::SID_COMM_TOWER)
	{
		maxAge(600);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);
	}


	int getRange() const
	{
		return operational() ? BaseRange : 0;
	}


	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Communication Range:";

		auto communicationRange = getRange();
		stringTable[{1, 0}].text = std::to_string(communicationRange);

		if (communicationRange == 0)
		{
			stringTable[{1, 0}].textColor = constants::WarningTextColor;
		}

		return stringTable;
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(3);
	}
};
