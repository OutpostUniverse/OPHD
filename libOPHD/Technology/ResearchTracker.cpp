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

	mTechnologiesBeingResearched.insert_or_assign(techId, ResearchProgress{progress, assigned});
}


void ResearchTracker::updateResearch(int techId, int progress, int assigned)
{
	if (std::find(mCompleted.begin(), mCompleted.end(), techId) != mCompleted.end())
	{
		throw std::runtime_error("Can't update a technology that's already researched.");
	}

	mTechnologiesBeingResearched.at(techId) = {progress, assigned};
}


const ResearchTracker::ResearchProgress& ResearchTracker::researchProgress(int techId) const
{
	return mTechnologiesBeingResearched.at(techId);
}
