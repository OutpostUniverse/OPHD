#include "ResearchTracker.h"

#include <algorithm>
#include <stdexcept>


bool ResearchTracker::isCompleted(int techId) const
{
	return std::find(mCompleted.begin(), mCompleted.end(), techId) != mCompleted.end();
}


bool ResearchTracker::isInProgress(int techId) const
{
	return mTechnologiesBeingResearched.contains(techId);
}


bool ResearchTracker::prerequisitesMet(const Technology& technology) const
{
	return std::ranges::all_of(technology.requiredTechnologies, [this](int techId) { return isCompleted(techId); });
}


void ResearchTracker::startResearch(int techId, int progress, int assigned)
{
	if (isCompleted(techId))
	{
		throw std::runtime_error("Can't update a technology that's already researched.");
	}

	mTechnologiesBeingResearched.insert_or_assign(techId, ResearchProgress{progress, assigned});
}


void ResearchTracker::updateResearch(int techId, int progress, int assigned)
{
	if (isCompleted(techId))
	{
		throw std::runtime_error("Can't update a technology that's already researched.");
	}

	mTechnologiesBeingResearched.at(techId) = {progress, assigned};
}


bool ResearchTracker::addProgress(const Technology& technology, int points)
{
	if (isCompleted(technology.id))
	{
		throw std::runtime_error("Can't add progress to a technology that's already researched.");
	}

	auto& researchProgress = mTechnologiesBeingResearched[technology.id];
	researchProgress.progress += points;

	if (researchProgress.progress < technology.cost)
	{
		return false;
	}

	mTechnologiesBeingResearched.erase(technology.id);
	mCompleted.push_back(technology.id);
	clearActiveResearch(technology);
	return true;
}


const ResearchTracker::ResearchProgress& ResearchTracker::researchProgress(int techId) const
{
	return mTechnologiesBeingResearched.at(techId);
}


std::optional<int> ResearchTracker::activeResearch(LabType labType) const
{
	const auto it = mActiveResearch.find(labType);
	if (it == mActiveResearch.end())
	{
		return std::nullopt;
	}

	return it->second;
}


void ResearchTracker::setActiveResearch(const Technology& technology)
{
	if (isCompleted(technology.id))
	{
		throw std::runtime_error("Can't research a technology that's already researched.");
	}

	if (!prerequisitesMet(technology))
	{
		throw std::runtime_error("Can't research a technology with unmet prerequisites.");
	}

	mActiveResearch.insert_or_assign(technology.labType, technology.id);
}


void ResearchTracker::clearActiveResearch(const Technology& technology)
{
	if (activeResearch(technology.labType) == technology.id)
	{
		mActiveResearch.erase(technology.labType);
	}
}
