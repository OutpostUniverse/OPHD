#include "ResearchFacility.h"

#include <algorithm>
#include <cmath>


ResearchFacility::ResearchFacility(StructureID id, Tile& tile) :
	Structure{id, tile}
{}


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
