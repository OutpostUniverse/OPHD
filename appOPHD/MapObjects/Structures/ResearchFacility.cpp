#include "ResearchFacility.h"

#include "../../UI/StringTable.h"

#include <algorithm>
#include <cmath>


ResearchFacility::ResearchFacility(StructureClass structureClass, StructureID id) :
	Structure(structureClass, id)
{}


StringTable ResearchFacility::createInspectorViewTable() const
{
	StringTable stringTable(2, 3);

	stringTable[{0, 0}].text = "Research Produced:";
	stringTable[{0, 1}].text = "Regular";
	stringTable[{0, 2}].text = "Hot";

	stringTable[{1, 1}].text = std::to_string(regularResearchProduced());
	stringTable[{1, 2}].text = std::to_string(hotResearchProduced());

	return stringTable;
}


int ResearchFacility::regularResearchProduced() const
{
	return static_cast<int>(
		std::floor(static_cast<float>(mActualScientistsEmployed) * mRegularPointsPerScientist));
}


int ResearchFacility::hotResearchProduced() const
{
	return static_cast<int>(
		std::floor(static_cast<float>(mActualScientistsEmployed) * mHotPointsPerScientist));
}


int ResearchFacility::scientistsNeeded() const
{
	return mMaxScientistsAllowed - mActualScientistsEmployed;
}


void ResearchFacility::assignScientists(int count)
{
	mActualScientistsEmployed = std::clamp(count, 0, mMaxScientistsAllowed);
}


int ResearchFacility::assignedScientists() const
{
	return mActualScientistsEmployed;
}


void ResearchFacility::maxScientistsAllowed(int count)
{
	mMaxScientistsAllowed = count;
}


void ResearchFacility::regularPointsPerScientist(float count)
{
	mRegularPointsPerScientist = count;
}


void ResearchFacility::hotPointsPerScientist(float count)
{
	mHotPointsPerScientist = count;
}
