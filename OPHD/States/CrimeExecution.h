#pragma once

#include "../UI/NotificationArea.h"
#include "../Things/Structures/Structures.h"
#include "../Common.h"
#include <vector>
#include <array>
#include <map>
#include <string>


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

	const static inline std::vector<std::string> stealingResoureReasons
	{
		"There are no identified suspects",
		"An investigation has been opened",
		"A local crime syndicate is under investigation",
		"A suspect was aprehended but the goods remain unaccounted for",
		"A separatist political movement has claimed responsibility",
		"The rebel faction is suspected in preparation for a splinter colony"
	};

	Difficulty mDifficulty{ Difficulty::Medium };
	NotificationArea& mNotificationArea;

	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
	int calcAmountForStealing(int unadjustedMin, int unadjustedMax);
	std::string getReasonForStealing();
};
