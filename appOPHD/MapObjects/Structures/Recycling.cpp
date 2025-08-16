#include "Recycling.h"

#include "../../Constants/UiConstants.h"

#include "../../UI/StringTable.h"

#include <libOPHD/EnumStructureID.h>


Recycling::Recycling(Tile& tile) :
	Structure{StructureID::Recycling, tile}
{
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
