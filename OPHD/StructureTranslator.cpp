// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StructureTranslator.h"

#include "Constants.h"

std::map<std::string, StructureID> StructureTranslator::_stringToStructureTable;
std::array<std::string, StructureID::SID_COUNT> StructureTranslator::_structureToStringTable;

/**
 * 
 */
StructureID StructureTranslator::translateFromString(const std::string& s)
{
	if (_stringToStructureTable.find(s) == _stringToStructureTable.end())
		return StructureID::SID_NONE;

	return _stringToStructureTable[s];
}


/**
 * 
 */
const std::string& StructureTranslator::translateToString(StructureID s)
{
	return _structureToStringTable[s];
}


/**
 * 
 */
void StructureTranslator::init()
{
	buildTables();
}


/**
 * 
 */
void StructureTranslator::buildTables()
{
	// String to ID
	_stringToStructureTable[constants::AGRIDOME] = StructureID::SID_AGRIDOME;
	_stringToStructureTable[constants::AIR_SHAFT] = StructureID::SID_AIR_SHAFT;
	_stringToStructureTable[constants::CARGO_LANDER] = StructureID::SID_CARGO_LANDER;
	_stringToStructureTable[constants::CHAP] = StructureID::SID_CHAP;
	_stringToStructureTable[constants::COLONIST_LANDER] = StructureID::SID_COLONIST_LANDER;
	_stringToStructureTable[constants::COMMAND_CENTER] = StructureID::SID_COMMAND_CENTER;
	_stringToStructureTable[constants::COMMERCIAL] = StructureID::SID_COMMERCIAL;
	_stringToStructureTable[constants::COMM_TOWER] = StructureID::SID_COMM_TOWER;
	_stringToStructureTable[constants::FUSION_REACTOR] = StructureID::SID_FUSION_REACTOR;
	_stringToStructureTable[constants::HOT_LABORATORY] = StructureID::SID_HOT_LABORATORY;
	_stringToStructureTable[constants::LABORATORY] = StructureID::SID_LABORATORY;
	_stringToStructureTable[constants::MEDICAL_CENTER] = StructureID::SID_MEDICAL_CENTER;
	_stringToStructureTable[constants::MINE_FACILITY] = StructureID::SID_MINE_FACILITY;
	_stringToStructureTable[constants::MINE_SHAFT] = StructureID::SID_MINE_SHAFT;
	_stringToStructureTable[constants::NURSERY] = StructureID::SID_NURSERY;
	_stringToStructureTable[constants::PARK] = StructureID::SID_PARK;
	_stringToStructureTable[constants::SURFACE_POLICE] = StructureID::SID_SURFACE_POLICE;
	_stringToStructureTable[constants::UNDERGROUND_POLICE] = StructureID::SID_UNDERGROUND_POLICE;
	_stringToStructureTable[constants::SEED_FACTORY] = StructureID::SID_SEED_FACTORY;
	_stringToStructureTable[constants::SEED_LANDER] = StructureID::SID_SEED_LANDER;
	_stringToStructureTable[constants::SEED_POWER] = StructureID::SID_SEED_POWER;
	_stringToStructureTable[constants::RECREATION_CENTER] = StructureID::SID_RECREATION_CENTER;
	_stringToStructureTable[constants::RED_LIGHT_DISTRICT] = StructureID::SID_RED_LIGHT_DISTRICT;
	_stringToStructureTable[constants::RESIDENCE] = StructureID::SID_RESIDENCE;
	_stringToStructureTable[constants::ROBOT_COMMAND] = StructureID::SID_ROBOT_COMMAND;
	_stringToStructureTable[constants::SEED_SMELTER] = StructureID::SID_SEED_SMELTER;
	_stringToStructureTable[constants::STORAGE_TANKS] = StructureID::SID_STORAGE_TANKS;
	_stringToStructureTable[constants::SURFACE_FACTORY] = StructureID::SID_SURFACE_FACTORY;
	_stringToStructureTable[constants::SMELTER] = StructureID::SID_SMELTER;
	_stringToStructureTable[constants::SOLAR_PANEL1] = StructureID::SID_SOLAR_PANEL1;
	_stringToStructureTable[constants::SOLAR_PLANT] = StructureID::SID_SOLAR_PLANT;
	_stringToStructureTable[constants::UNDERGROUND_FACTORY] = StructureID::SID_UNDERGROUND_FACTORY;
	_stringToStructureTable[constants::UNIVERSITY] = StructureID::SID_UNIVERSITY;
	_stringToStructureTable[constants::WAREHOUSE] = StructureID::SID_WAREHOUSE;

	_stringToStructureTable[constants::TUBE] = StructureID::SID_TUBE;

	// ID To String
	_structureToStringTable[StructureID::SID_AGRIDOME] = constants::AGRIDOME;
	_structureToStringTable[StructureID::SID_AIR_SHAFT] = constants::AIR_SHAFT;
	_structureToStringTable[StructureID::SID_CARGO_LANDER] = constants::CARGO_LANDER;
	_structureToStringTable[StructureID::SID_CHAP] = constants::CHAP;
	_structureToStringTable[StructureID::SID_COLONIST_LANDER] = constants::COLONIST_LANDER;
	_structureToStringTable[StructureID::SID_COMMAND_CENTER] = constants::COMMAND_CENTER;
	_structureToStringTable[StructureID::SID_COMMERCIAL] = constants::COMMERCIAL;
	_structureToStringTable[StructureID::SID_COMM_TOWER] = constants::COMM_TOWER;
	_structureToStringTable[StructureID::SID_FUSION_REACTOR] = constants::FUSION_REACTOR;
	_structureToStringTable[StructureID::SID_HOT_LABORATORY] = constants::HOT_LABORATORY;
	_structureToStringTable[StructureID::SID_LABORATORY] = constants::LABORATORY;
	_structureToStringTable[StructureID::SID_MEDICAL_CENTER] = constants::MEDICAL_CENTER;
	_structureToStringTable[StructureID::SID_MINE_FACILITY] = constants::MINE_FACILITY;
	_structureToStringTable[StructureID::SID_MINE_SHAFT] = constants::MINE_SHAFT;
	_structureToStringTable[StructureID::SID_NURSERY] = constants::NURSERY;
	_structureToStringTable[StructureID::SID_PARK] = constants::PARK;
	_structureToStringTable[StructureID::SID_SURFACE_POLICE] = constants::SURFACE_POLICE;
	_structureToStringTable[StructureID::SID_UNDERGROUND_POLICE] = constants::UNDERGROUND_POLICE;
	_structureToStringTable[StructureID::SID_RECREATION_CENTER] = constants::RECREATION_CENTER;
	_structureToStringTable[StructureID::SID_RED_LIGHT_DISTRICT] = constants::RED_LIGHT_DISTRICT;
	_structureToStringTable[StructureID::SID_RESIDENCE] = constants::RESIDENCE;
	_structureToStringTable[StructureID::SID_ROBOT_COMMAND] = constants::ROBOT_COMMAND;
	_structureToStringTable[StructureID::SID_SEED_FACTORY] = constants::SEED_FACTORY;
	_structureToStringTable[StructureID::SID_SEED_LANDER] = constants::SEED_LANDER;
	_structureToStringTable[StructureID::SID_SEED_POWER] = constants::SEED_POWER;
	_structureToStringTable[StructureID::SID_SEED_SMELTER] = constants::SEED_SMELTER;
	_structureToStringTable[StructureID::SID_SMELTER] = constants::SMELTER;
	_structureToStringTable[StructureID::SID_SOLAR_PANEL1] = constants::SOLAR_PANEL1;
	_structureToStringTable[StructureID::SID_SOLAR_PLANT] = constants::SOLAR_PLANT;
	_structureToStringTable[StructureID::SID_STORAGE_TANKS] = constants::STORAGE_TANKS;
	_structureToStringTable[StructureID::SID_SURFACE_FACTORY] = constants::SURFACE_FACTORY;
	_structureToStringTable[StructureID::SID_UNDERGROUND_FACTORY] = constants::UNDERGROUND_FACTORY;
	_structureToStringTable[StructureID::SID_UNIVERSITY] = constants::UNIVERSITY;
	_structureToStringTable[StructureID::SID_WAREHOUSE] = constants::WAREHOUSE;

	_structureToStringTable[StructureID::SID_TUBE] = constants::TUBE;
}
