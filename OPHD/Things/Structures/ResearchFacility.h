#pragma once

#include "Structure.h"

class ResearchFacility : public Structure
{
public:
	ResearchFacility(const std::string& name, const std::string& spritePath, StructureClass structureClass, StructureID id) :
		Structure(name, spritePath, structureClass, id) {}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 3);

		stringTable[{0, 0}].text = "Research Produced:";
		stringTable[{0, 1}].text = "Regular";
		stringTable[{0, 2}].text = "Hot";

		stringTable[{1, 1}].text = std::to_string(regularResearchProduced());
		stringTable[{1, 2}].text = std::to_string(hotResearchProduced());

		return stringTable;
	}

	int regularResearchProduced() const { return mRegularResearchProduced; }
	int hotResearchProduced() const { return mHotResearchProduced; }

private:
	int mRegularResearchProduced{0};
	int mHotResearchProduced{0};
};