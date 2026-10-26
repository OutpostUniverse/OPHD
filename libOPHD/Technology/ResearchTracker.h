#pragma once

#include "Technology.h"

#include <map>
#include <optional>
#include <vector>


class ResearchTracker
{
public:
	struct ResearchProgress
	{
		int progress;
		int scientistsAssigned;
	};


	const std::vector<int>& completedResearch() const
	{
		return mCompleted;
	}


	const std::map<int, ResearchProgress>& currentResearch() const
	{
		return mTechnologiesBeingResearched;
	}


	void addCompletedResearch(int techId)
	{
		mCompleted.push_back(techId);
	}


	bool isCompleted(int techId) const;
	bool isInProgress(int techId) const;
	bool prerequisitesMet(const Technology& technology) const;

	void startResearch(int techId, int progress, int assigned);
	void updateResearch(int techId, int progress, int assigned);
	bool addProgress(const Technology& technology, int points);
	const ResearchProgress& researchProgress(int techId) const;

	std::optional<int> activeResearch(int labType) const;
	void setActiveResearch(const Technology& technology);


private:
	void clearActiveResearch(const Technology& technology);

	std::vector<int> mCompleted;
	std::map<int, ResearchProgress> mTechnologiesBeingResearched;
	std::map<int, int> mActiveResearch;
};
