#pragma once

#include "../UI/NotificationArea.h"
#include "../Things/Structures/Structures.h"
#include "../Common.h"
#include <vector>
#include <array>
#include <map>


class CrimeExecution 
{
public:
	CrimeExecution(NotificationArea& notificationArea);

	void difficulty(Difficulty difficulty) { mDifficulty = difficulty; }

	void executeCrimes(const std::vector<Structure*>& structuresCommittingCrime);

	void stealFood(FoodProduction& structure);
	void stealRefinedResources(Structure& structure);
	void stealRawResources(Structure& structure);

private:
	const static inline std::map<Difficulty, double> stealingMultipliers
	{
		{Difficulty::Beginner, 0.5f},
		{Difficulty::Easy, 0.75f},
		{Difficulty::Medium, 1.0f},
		{Difficulty::Hard, 1.5f}
	};

	Difficulty mDifficulty{ Difficulty::Medium };
	NotificationArea& mNotificationArea;

	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
	int calcAmountForStealing(int unadjustedMin, int unadjustedMax);
};
