#pragma once


#include "Structure.h"
#include <string>

/**
 * \class	PowerStructure
 * \brief	Virtual class for structures whose primary purpose is power production
 *
 * \note	PowerStructure is an abstract class
 */
class OreRefining : public Structure
{
public:
	OreRefining(const std::string& name, const std::string& spritePath, StructureClass structureClass) :
		Structure(name, spritePath, structureClass) {}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Storage Capacity:";
		stringTable[{1, 0}].text = std::to_string(calculateMaxStorage());

		return stringTable;
	}

protected:
	virtual int calculateMaxStorage() = 0;
};
