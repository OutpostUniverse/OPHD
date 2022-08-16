#pragma once

#include "Structure.h"

#include <cmath>


class ResearchFacility : public Structure
{
public:
	ResearchFacility(const std::string& name, const std::string& spritePath, StructureClass structureClass, StructureID id) :
		Structure(name, spritePath, structureClass, id)
	{}


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


	int regularResearchProduced() const
	{
		return static_cast<int>(
			std::floor(mActualScientstsEmployed * mRegularPointsPerScientist));
	}

	int hotResearchProduced() const
	{
		return static_cast<int>(
			std::floor(mActualScientstsEmployed * mHotPointsPerScientist));
	}


	int scientistsNeeded() const { return mMaxScientstsAllowed - mActualScientstsEmployed; }

	void assignScientsts(int count) { mActualScientstsEmployed = count; }


protected:
	void maxScientistsAllowed(int count) { mMaxScientstsAllowed = count; }

	void regularPointsPerScientist(float count) { mRegularPointsPerScientist = count; }
	void hotPointsPerScientist(float count) { mHotPointsPerScientist = count; }


private:
	int mMaxScientstsAllowed{0};
	int mActualScientstsEmployed{0};

	float mRegularPointsPerScientist{0.0f};
	float mHotPointsPerScientist{0.0f};
};
