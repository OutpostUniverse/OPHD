#include "Recycling.h"

#include "../../Constants/UiConstants.h"

#include "../../UI/StringTable.h"

#include <libOPHD/EnumStructureID.h>


namespace
{
	const int WasteProcessingCapacity = 30;
	const int ResidentialSupportCount = 10;
}


Recycling::Recycling(Tile& tile) :
	Structure{StructureID::Recycling, tile}
{
}


int Recycling::wasteProcessingCapacity() const
{
	return operational() ? WasteProcessingCapacity : 0;
}


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
