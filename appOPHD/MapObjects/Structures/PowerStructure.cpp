#include "PowerStructure.h"

#include "../../Constants/UiConstants.h"

#include "../../UI/StringTable.h"


PowerStructure::PowerStructure(StructureID id, Tile& tile) :
	Structure{id, tile}
{
}


StringTable PowerStructure::createInspectorViewTable() const
{
	StringTable stringTable(2, 1);

	stringTable[{0, 0}].text = "Power Produced:";

	auto produced = energyProduced();

	stringTable[{1, 0}].text = std::to_string(produced) + " / " + std::to_string(calculateMaxEnergyProduction());

	if (produced == 0)
	{
		stringTable[{1, 0}].textColor = constants::WarningTextColor;
	}

	return stringTable;
}


int PowerStructure::energyProduced() const
{
	return operational() ? calculateMaxEnergyProduction() : 0;
}
