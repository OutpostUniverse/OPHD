#pragma once

#include "Technology.h"

#include "../Common.h"

#include <map>
#include <tuple>
#include <vector>

class ResearchTracker
{
public:
	ResearchTracker() = default;
	~ResearchTracker() = default;


	const std::vector<int>& completedResearch() const
	{
		return mCompleted;
	}

	
	const std::map<int, std::tuple<int, int>>& currentResearch() const
	{
		return mTechnologiesBeingResearched;
	}


	void addCompletedResearch(int techId)
	{
		mCompleted.push_back(techId);
	}


	void startResearch(int techId, int progress, int assigned);
	void updateResearch(int techId, int progress, int assigned);
	const std::tuple<int, int>& researchProgress(int techId) const;


private:
	std::vector<int> mCompleted;
	std::map<int, std::tuple<int, int>> mTechnologiesBeingResearched;
};
