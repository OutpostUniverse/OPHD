#include "ResearchTracker.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>


void ResearchTracker::startResearch(int techId, int progress, int assigned)
{
	if (std::find(mCompleted.begin(), mCompleted.end(), techId) != mCompleted.end())
	{
		throw std::runtime_error("Can't update a technology that's already researched.");
	}

	auto emplacedItem = mTechnologiesBeingResearched.emplace(techId, std::make_tuple(progress, assigned));
	if (!emplacedItem.second)
	{
		(*emplacedItem.first).second = {progress, assigned};
	}

}


void ResearchTracker::updateResearch(int techId, int progress, int assigned)
{
	if (std::find(mCompleted.begin(), mCompleted.end(), techId) != mCompleted.end())
	{
		throw std::runtime_error("Can't update a technology that's already researched.");
	}

	mTechnologiesBeingResearched.at(techId) = {progress, assigned};
}


const std::tuple<int, int>& ResearchTracker::researchProgress(int techId) const
{
	return mTechnologiesBeingResearched.at(techId);
}
