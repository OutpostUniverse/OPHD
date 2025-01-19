#include "Common.h"
#include "Constants/Numbers.h"
#include "EnumMineProductionRate.h"
#include "EnumMoraleIndex.h"
#include "EnumTerrainType.h"

#include "MapObjects/Structures/Warehouse.h"

#include <libOPHD/XmlSerializer.h>

#include <NAS2D/Xml/XmlDocument.h>
#include <NAS2D/Xml/XmlElement.h>

#include <stdexcept>
#include <algorithm>


using namespace NAS2D;


namespace
{
	float meanSolarDistance = 1;
}


const std::map<TerrainType, std::string> TILE_INDEX_TRANSLATION =
{
	{TerrainType::Dozed, constants::TileBulldozed},
	{TerrainType::Clear, constants::TileClear},
	{TerrainType::Rough, constants::TileRough},
	{TerrainType::Difficult, constants::TileDifficult},
	{TerrainType::Impassable, constants::TileImpassable},
};


const std::map<MineProductionRate, std::string> MINE_YIELD_TRANSLATION =
{
	{MineProductionRate::High, constants::MineYieldHigh},
	{MineProductionRate::Low, constants::MineYieldLow},
	{MineProductionRate::Medium, constants::MineYieldMedium}
};


const std::array MoraleStringTable =
{
	std::string("Terrible"),
	std::string("Poor"),
	std::string("Fair"),
	std::string("Good"),
	std::string("Excellent"),

	std::string("Morale is "),
	std::string("Births"),
	std::string("Deaths"),
	std::string("No active Food Production"),
	std::string("Parks & Arboretums"),
	std::string("Recreational Facilities"),
	std::string("Luxury Availability"),
	std::string("Residential Over Capacity"),
	std::string("Biowaste Overflowing"),
	std::string("Structures Disabled"),
	std::string("Structures Destroyed")
};


const std::string& moraleString(std::size_t index)
{
	return MoraleStringTable[index];
}


const std::string& moraleString(MoraleIndexs morale)
{
	return MoraleStringTable[static_cast<std::size_t>(morale)];
}


std::size_t moraleStringTableCount()
{
	return MoraleStringTable.size();
}


void checkSavegameVersion(const std::string& filename)
{
	// openSavegame checks version number after opening file
	openSavegame(filename);
}


/**
 * Open a saved game and validate version.
 *
 * \throws	Throws a std::runtime_error if there are any errors with a savegame version, formation or missing root nodes.
 */
NAS2D::Xml::XmlDocument openSavegame(const std::string& filename)
{
	auto xmlDocument = openXmlFile(filename, constants::SaveGameRootNode);

	auto savegameVersion = xmlDocument.firstChildElement(constants::SaveGameRootNode)->attribute("version");

	if (savegameVersion != constants::SaveGameVersion)
	{
		throw std::runtime_error("Savegame version mismatch: '" + filename + "'. Expected " + constants::SaveGameVersion + ", found " + savegameVersion + ".");
	}

	return xmlDocument;
}


void setMeanSolarDistance(float newMeanSolarDistance)
{
	if (newMeanSolarDistance <= 0)
	{
		throw std::runtime_error("Must set a positive value for `meanSolarDistance`: " + std::to_string(newMeanSolarDistance));
	}
	meanSolarDistance = newMeanSolarDistance;
}


float getMeanSolarDistance()
{
	return meanSolarDistance;
}
