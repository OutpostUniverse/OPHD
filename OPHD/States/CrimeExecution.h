#pragma once

#include "../UI/NotificationArea.h"
#include "../Things/Structures/Structures.h"
#include "../Common.h"
#include <vector>


class CrimeExecution 
{
public:
	CrimeExecution(NotificationArea& notificationArea);

	void difficulty(Difficulty difficulty) { mDifficulty = difficulty; }

	void executeCrimes(const std::vector<Structure*>& structuresCommittingCrime);

	void stealFood(FoodProduction& structure);

private:
	Difficulty mDifficulty{ Difficulty::Medium };
	NotificationArea& mNotificationArea;
};
