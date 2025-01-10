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
	CrimeExecution(NotificationArea& notificationArea, const Difficulty& difficulty);

	void executeCrimes(const std::vector<Structure*>& structuresCommittingCrime);

	std::vector<std::pair<std::string, int>> moraleChanges() const { return mMoraleChanges; }

protected:
	void stealFood(FoodProduction& structure);
	void stealRefinedResources(Structure& structure);
	void stealRawResources(Structure& structure);
	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
	void vandalize(Structure& structure);

private:
	const Difficulty& mDifficulty;
	NotificationArea& mNotificationArea;
	std::vector<std::pair<std::string, int>> mMoraleChanges;
};
