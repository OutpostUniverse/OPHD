#pragma once

#include "../Structure.h"

#include "../../Constants/UiConstants.h"

#include <string>


/**
 * Virtual class for structures whose primary purpose is power production
 *
 * \note	PowerStructure is an abstract class
 */
class PowerStructure : public Structure
{
public:
	PowerStructure(const std::string& name, const std::string& spritePath, StructureClass structureClass, StructureID id) :
		Structure(name, spritePath, structureClass, id) {}

	StringTable createInspectorViewTable() override
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

	int energyProduced()
	{
		return operational() ? calculateMaxEnergyProduction() : 0;
	}

protected:
	virtual int calculateMaxEnergyProduction() = 0;
};
