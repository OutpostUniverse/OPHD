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
	StringTable stringTable(2, 1);

	stringTable[{0, 0}].text = "Max Waste Processing Capacity:";
	stringTable[{1, 0}].text = std::to_string(bioWasteProcessingCapacity());

	if (!operational()) {
		stringTable[{1, 0}].textColor = constants::WarningTextColor;
	}

	return stringTable;
}
