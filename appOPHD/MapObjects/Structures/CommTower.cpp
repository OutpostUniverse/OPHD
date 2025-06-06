#include "CommTower.h"

#include "../../Constants/UiConstants.h"

#include "../../UI/StringTable.h"


CommTower::CommTower() : Structure(
	StructureClass::Communication,
	StructureID::SID_COMM_TOWER)
{
}


int CommTower::getRange() const
{
	return commRange();
}


StringTable CommTower::createInspectorViewTable()
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
