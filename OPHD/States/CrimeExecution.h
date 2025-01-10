#pragma once

#include "../MapObjects/Structures/FoodProduction.h"
#include "../Common.h"

#include <vector>
#include <array>
#include <map>
#include <string>
#include <utility>


class NotificationArea;


class CrimeExecution
{
public:
	CrimeExecution(NotificationArea& notificationArea, Difficulty difficulty);

	void difficulty(Difficulty difficulty) { mDifficulty = difficulty; }

	void executeCrimes(const std::vector<Structure*>& structuresCommittingCrime);

	void stealFood(FoodProduction& structure);
	void stealRefinedResources(Structure& structure);
	void stealRawResources(Structure& structure);
	void vandalize(Structure& structure);

	std::vector<std::pair<std::string, int>> moraleChanges() const { return mMoraleChanges; }

private:
	Difficulty mDifficulty;
	NotificationArea& mNotificationArea;
	std::vector<std::pair<std::string, int>> mMoraleChanges;

	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
};
