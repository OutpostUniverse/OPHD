#pragma once

#include "../Structure.h"

#include <algorithm>
#include <cmath>


class ResearchFacility : public Structure
{
public:
	ResearchFacility(StructureClass structureClass, StructureID id) :
		Structure(structureClass, id)
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
			std::floor(static_cast<float>(mActualScientistsEmployed) * mRegularPointsPerScientist));
	}


	int hotResearchProduced() const
	{
		return static_cast<int>(
			std::floor(static_cast<float>(mActualScientistsEmployed) * mHotPointsPerScientist));
	}


	int scientistsNeeded() const
	{
		return mMaxScientistsAllowed - mActualScientistsEmployed;
	}


	void assignScientists(int count)
	{
		mActualScientistsEmployed = std::clamp(count, 0, mMaxScientistsAllowed);
	}


	int assignedScientists() const
	{
		return mActualScientistsEmployed;
	}


protected:
	void maxScientistsAllowed(int count) { mMaxScientistsAllowed = count; }

	void regularPointsPerScientist(float count) { mRegularPointsPerScientist = count; }
	void hotPointsPerScientist(float count) { mHotPointsPerScientist = count; }


private:
	int mMaxScientistsAllowed{0};
	int mActualScientistsEmployed{0};

	float mRegularPointsPerScientist{0.0f};
	float mHotPointsPerScientist{0.0f};
};
