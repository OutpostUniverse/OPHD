#include "Recycling.h"

#include "../../Constants/UiConstants.h"

#include "../../UI/StringTable.h"


namespace
{
	const int WasteProcessingCapacity = 30;
	const int ResidentialSupportCount = 10;
}


Recycling::Recycling(Tile& tile) :
	Structure{StructureID::SID_RECYCLING, tile}
{
}


/**
 * Amount of waste the facility can process per turn.
 */
int Recycling::wasteProcessingCapacity() const
{
	return operational() ? WasteProcessingCapacity : 0;
}


/**
 * Number of residential units the facility can support
 * each turn.
 */
int Recycling::residentialSupportCount() const
{
	return operational() ? ResidentialSupportCount : 0;
}


StringTable Recycling::createInspectorViewTable() const
{
	StringTable stringTable(2, 2);

	stringTable[{0, 0}].text = "Max Residents Supported:";
	stringTable[{1, 0}].text = std::to_string(residentialSupportCount());

	stringTable[{0, 1}].text = "Max Waste Processing Capacity:";
	stringTable[{1, 1}].text = std::to_string(wasteProcessingCapacity());

	if (!operational()) {
		stringTable[{1, 0}].textColor = constants::WarningTextColor;
		stringTable[{1, 1}].textColor = constants::WarningTextColor;
	}

	return stringTable;
}
