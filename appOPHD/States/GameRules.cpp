#include "GameRules.h"

namespace
{
	constexpr inline int ColonyShipDeorbitTurn = 24;
	constexpr inline int ColonistsPerLander = 50;
	constexpr inline int CargoLanderMoraleLossMultiplier = 25;
	const std::map<Difficulty, int> DifficultyDeorbitMoraleLossMultiplier{
		{Difficulty::Beginner, 1},
		{Difficulty::Easy, 3},
		{Difficulty::Medium, 6},
		{Difficulty::Hard, 10}};
}


bool GameRules::colonyShipDeorbit(const int turnCount) const
{
	return turnCount == ColonyShipDeorbitTurn;
}


const ColonyShipDeorbitEffects GameRules::colonyShipDeorbitEffects(const Difficulty difficulty, const int colonistLanders, const int cargoLanders) const
{
	ColonyShipDeorbitEffects effects;
	effects.moraleChangeEntries.push_back(colonyShipCrashMoraleChangeEntries({"Deorbit (Colonists)", colonistLanders, ColonistsPerLander}, difficulty));
	effects.moraleChangeEntries.push_back(colonyShipCrashMoraleChangeEntries({"Deorbit (Cargo)", cargoLanders, CargoLanderMoraleLossMultiplier}, difficulty));

	effects.announcementType = colonyShipCrashAnnouncement(colonistLanders, cargoLanders);

	return effects;
}


MajorEventAnnouncement::AnnouncementType GameRules::colonyShipCrashAnnouncement(const int colonistLanders, const int cargoLanders) const
{
	if (colonistLanders && cargoLanders)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithColonistsAndCargo;
	
	if (colonistLanders)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithColonists;

	if (cargoLanders)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithCargo;
	
	return MajorEventAnnouncement::AnnouncementType::ColonyShipCrash;
}


const MoraleChangeEntry GameRules::colonyShipCrashMoraleChangeEntries(std::tuple<std::string, int, int> entry, Difficulty difficulty) const
{
	const auto [description, landers, multiplier] = entry;

	auto difficultyMultiplier = DifficultyDeorbitMoraleLossMultiplier.find(difficulty);
	if (difficultyMultiplier != DifficultyDeorbitMoraleLossMultiplier.end())
	{
		return MoraleChangeEntry{description, -1 * landers * multiplier * difficultyMultiplier->second};
	}

	throw std::out_of_range("Invalid difficulty level");
}
