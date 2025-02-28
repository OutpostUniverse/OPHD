#pragma once

#include "../UI/MajorEventAnnouncement.h"

#include <libOPHD/Population/Morale.h>
#include <libOPHD/EnumDifficulty.h>

#include <optional>
#include <map>
#include <string>
#include <vector>

struct ColonyShipDeorbitEffects
{
	std::vector<MoraleChangeEntry> moraleChangeEntries;
	MajorEventAnnouncement::AnnouncementType announcementType = {};
	const int colonistLanders = 0;
	const int cargoLanders = 0;
};

class GameRules
{
public:
	bool colonyShipDeorbit(const int turnCount) const;
	const ColonyShipDeorbitEffects colonyShipDeorbitEffects(const Difficulty difficulty, const int colonistLanders, const int cargoLanders) const;
	MajorEventAnnouncement::AnnouncementType colonyShipCrashAnnouncement(const int colonistLanders, const int cargoLanders) const;
	const MoraleChangeEntry colonyShipCrashMoraleChangeEntries(const std::tuple<std::string, int, int> entry, const Difficulty difficulty) const;
};
