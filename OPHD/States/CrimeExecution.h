#pragma once

#include "../UI/NotificationArea.h"
#include "../Things/Structures/Structures.h"
#include <vector>


class CrimeExecution 
{
public:
	CrimeExecution(NotificationArea& notificationArea);

	void executeCrimes(std::vector<Structure*> structuresCommittingCrime);
	void executeCrimes(std::vector<Structure*>&& structuresCommittingCrime);

	void executeStealingFood(FoodProduction& structure);

private:
	NotificationArea& mNotificationArea;
};
