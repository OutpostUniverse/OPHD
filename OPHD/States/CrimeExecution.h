#pragma once

#include "../UI/NotificationArea.h"
#include "../Things/Structures/Structures.h"
#include "../Common.h"
#include <vector>
#include <array>


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
	Difficulty mDifficulty{ Difficulty::Medium };
	NotificationArea& mNotificationArea;

	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
};
