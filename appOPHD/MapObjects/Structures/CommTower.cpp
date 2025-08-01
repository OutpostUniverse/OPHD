#include "CommTower.h"

#include "../../Constants/UiConstants.h"

#include "../../UI/StringTable.h"

#include <libOPHD/EnumStructureID.h>


CommTower::CommTower(Tile& tile) :
	Structure{StructureID::CommTower, tile}
{
}


StringTable CommTower::createInspectorViewTable() const
{
	StringTable stringTable(2, 1);

	stringTable[{0, 0}].text = "Communication Range:";

	auto communicationRange = commRange();
	stringTable[{1, 0}].text = std::to_string(communicationRange);

	if (communicationRange == 0)
	{
		stringTable[{1, 0}].textColor = constants::WarningTextColor;
	}

	return stringTable;
}
