#include "ColonyShip.h"

#include "appOPHD/Constants/Strings.h"
#include "appOPHD/OpenSaveGame.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/ParserHelper.h>

#include <libOPHD/EnumDifficulty.h>

#include <map>
#include <stdexcept>
namespace
{
	const std::map<Difficulty, int> DifficultyDeorbitMoraleLossMultiplier{
		{Difficulty::Beginner, 1},
		{Difficulty::Easy, 3},
		{Difficulty::Medium, 6},
		{Difficulty::Hard, 10}};

	constexpr inline int ColonistsPerLander = 50;
	constexpr inline int CargoMoraleLossPerLander = 25;
		
	void setLanders(ColonyShipData& colonyShipData, NAS2D::Xml::XmlElement* element)
	{
		if (element)
		{
			const auto dictionary = NAS2D::attributesToDictionary(*element);
			colonyShipData.colonistLanders = dictionary.get<int>("colonist_landers");
			colonyShipData.cargoLanders = dictionary.get<int>("cargo_landers");
		}
	}

	void setManeuveringFuel(ColonyShipData& colonyShipData, NAS2D::Xml::XmlElement* element)
	{
		if (element)
		{
			auto turnCount = NAS2D::attributesToDictionary(*element).get<int>("count");

			colonyShipData.turnsOfManeuveringFuel = turnCount > constants::ColonyShipOrbitTime ? 0 : constants::ColonyShipOrbitTime - turnCount + 1;
		}
	}
}


ColonyShipData colonyShipDataFromFile(const std::string& filePath)
{
	if (!NAS2D::Utility<NAS2D::Filesystem>::get().exists(filePath))
	{
		throw std::runtime_error("File '" + filePath + "' was not found.");
	}
	auto xmlDocument = openSavegame(filePath);
	auto* root = xmlDocument.firstChildElement(constants::SaveGameRootNode);

	ColonyShipData colonyShipData;
	setLanders(colonyShipData, root->firstChildElement("population"));
	setManeuveringFuel(colonyShipData, root->firstChildElement("turns"));

	colonyShipData.crashed = colonyShipData.turnsOfManeuveringFuel == 0;

	return colonyShipData;
}


ColonyShip::ColonyShip()
{}


ColonyShip::ColonyShip(const ColonyShipData& colonyShipData) :
	mColonistLanders{colonyShipData.colonistLanders},
	mCargoLanders{colonyShipData.cargoLanders},
	mTurnsOfManeuveringFuel{colonyShipData.turnsOfManeuveringFuel},
	mCrashed{colonyShipData.crashed}
{}


ColonyShip::ColonyShipCrashEffects ColonyShip::colonyShipCrashEffects(Difficulty difficulty)
{
	ColonyShipCrashEffects effects;
	effects.announcementType = colonyShipCrashAnnouncement(mColonistLanders, mCargoLanders);
	effects.moraleChangeEntries.push_back(colonyShipCrashMoraleChangeEntries(LanderCrashData{"Colonists lost in crash:", mColonistLanders, ColonistsPerLander}, difficulty));
	effects.moraleChangeEntries.push_back(colonyShipCrashMoraleChangeEntries(LanderCrashData{"Cargo lost in crash:", mCargoLanders, CargoMoraleLossPerLander}, difficulty));
	return effects;
}


MajorEventAnnouncement::AnnouncementType ColonyShip::colonyShipCrashAnnouncement(const int colonistLanders, const int cargoLanders)
{
	if (colonistLanders && cargoLanders)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithColonistsAndCargo;

	if (colonistLanders)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithColonists;

	if (cargoLanders)
		return MajorEventAnnouncement::AnnouncementType::ColonyShipCrashWithCargo;

	return MajorEventAnnouncement::AnnouncementType::ColonyShipCrash;
}


MoraleChangeEntry ColonyShip::colonyShipCrashMoraleChangeEntries(LanderCrashData landerCrashData, Difficulty difficulty)
{
	auto difficultyMultiplier = DifficultyDeorbitMoraleLossMultiplier.find(difficulty);
	if (difficultyMultiplier != DifficultyDeorbitMoraleLossMultiplier.end())
	{
		return MoraleChangeEntry{landerCrashData.description, -1 * landerCrashData.numberOfLanders * landerCrashData.landerMoraleLossMultiplier * difficultyMultiplier->second};
	}

	throw std::out_of_range("Invalid difficulty level");
}
