#pragma once

#include "Technology.h"

#include "../Common.h"

#include <tuple>
#include <vector>

class ResearchTracker
{
public:
	struct ResearchProgress
	{
		int technologyId{0};
		int progress{0};
		int scientistsAssigned{0};
	};


public:
	ResearchTracker() = default;
	~ResearchTracker() = default;


	const std::vector<int>& completedResearch() const
	{
		return mTechnologiesResearched;
	}

	
	const std::vector<ResearchProgress>& currentResearch() const
	{
		return mTechnologiesBeingResearched;
	}

	void updateResearch(const ResearchProgress& progress);
	ResearchProgress& researchProgress(int techId);


private:
	std::vector<int> mTechnologiesResearched;
	std::vector<ResearchProgress> mTechnologiesBeingResearched;
};
