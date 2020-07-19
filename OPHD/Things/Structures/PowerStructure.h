#pragma once

#include "Structure.h"
#include "../../Constants.h"
#include <string>

// 
/**
 * \class	PowerStructure
 * \brief	Virtual class for structures whose primary purpose is power production
 *
 * \note	PowerStructure is an abstract class
 */
class PowerStructure : public Structure
{
public:
	PowerStructure(const std::string& name, const std::string& spritePath, StructureClass structureClass) :
		Structure(name, spritePath, structureClass) {}
	virtual ~PowerStructure() {}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Power Produced:";

		auto energyProduced = calculateEnergyProduced();

		stringTable[{1, 0}].text = std::to_string(energyProduced) + " / " + std::to_string(calculateMaxEnergyProduction());

		if (energyProduced == 0)
		{
			stringTable[{1, 0}].textColor = constants::WARNING_TEXT_COLOR;
		}

		return stringTable;
	}

protected:
	virtual int calculateEnergyProduced() = 0;
	virtual int calculateMaxEnergyProduction() = 0;
};
