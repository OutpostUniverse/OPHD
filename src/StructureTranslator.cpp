#include "StructureTranslator.h"

#include "Constants.h"

std::map<std::string, StructureID> StructureTranslator::_stringToStructureTable;
std::array<std::string, SID_COUNT> StructureTranslator::_structureToStringTable;

StructureID StructureTranslator::translateFromString(const std::string& _s)
{
	if (_stringToStructureTable.find(_s) == _stringToStructureTable.end())
		return SID_NONE;

	return _stringToStructureTable[_s];
}


const std::string& StructureTranslator::translateToString(StructureID _s)
{
	return _structureToStringTable[_s];
}


void StructureTranslator::init()
{
	buildTables();
}


void StructureTranslator::buildTables()
{
	// String to ID
	_stringToStructureTable[constants::AGRIDOME] = SID_AGRIDOME;
	_stringToStructureTable[constants::AIR_SHAFT] = SID_AIR_SHAFT;
	_stringToStructureTable[constants::CHAP] = SID_CHAP;
	_stringToStructureTable[constants::COLONIST_LANDER] = SID_COLONIST_LANDER;
	_stringToStructureTable[constants::COMMAND_CENTER] = SID_COMMAND_CENTER;
	_stringToStructureTable[constants::COMMERCIAL] = SID_COMMERCIAL;
	_stringToStructureTable[constants::FUSION_REACTOR] = SID_FUSION_REACTOR;
	_stringToStructureTable[constants::HOT_LABORATORY] = SID_HOT_LABORATORY;
	_stringToStructureTable[constants::LABORATORY] = SID_LABORATORY;
	_stringToStructureTable[constants::MEDICAL_CENTER] = SID_MEDICAL_CENTER;
	_stringToStructureTable[constants::MINE_FACILITY] = SID_MINE_FACILITY;
	_stringToStructureTable[constants::MINE_SHAFT] = SID_MINE_SHAFT;
	_stringToStructureTable[constants::PARK] = SID_PARK;
	_stringToStructureTable[constants::SURFACE_POLICE] = SID_SURFACE_POLICE;
	_stringToStructureTable[constants::UNDERGROUND_POLICE] = SID_UNDERGROUND_POLICE;
	_stringToStructureTable[constants::SEED_FACTORY] = SID_SEED_FACTORY;
	_stringToStructureTable[constants::SEED_LANDER] = SID_SEED_LANDER;
	_stringToStructureTable[constants::SEED_POWER] = SID_SEED_POWER;
	_stringToStructureTable[constants::RECREATION_CENTER] = SID_RECREATION_CENTER;
	_stringToStructureTable[constants::RED_LIGHT_DISTRICT] = SID_RED_LIGHT_DISTRICT;
	_stringToStructureTable[constants::RESIDENCE] = SID_RESIDENCE;
	_stringToStructureTable[constants::ROBOT_COMMAND] = SID_ROBOT_COMMAND;
	_stringToStructureTable[constants::SEED_SMELTER] = SID_SEED_SMELTER;
	_stringToStructureTable[constants::STORAGE_TANKS] = SID_STORAGE_TANKS;
	_stringToStructureTable[constants::SURFACE_FACTORY] = SID_SURFACE_FACTORY;
	_stringToStructureTable[constants::SMELTER] = SID_SMELTER;
	_stringToStructureTable[constants::SOLAR_PLANT] = SID_SOLAR_PLANT;
	_stringToStructureTable[constants::UNDERGROUND_FACTORY] = SID_UNDERGROUND_FACTORY;
	_stringToStructureTable[constants::UNIVERSITY] = SID_UNIVERSITY;
	_stringToStructureTable[constants::WAREHOUSE] = SID_WAREHOUSE;

	_stringToStructureTable[constants::TUBE] = SID_TUBE;

	// ID To String
	_structureToStringTable[SID_AGRIDOME] = constants::AGRIDOME;
	_structureToStringTable[SID_AIR_SHAFT] = constants::AIR_SHAFT;
	_structureToStringTable[SID_CHAP] = constants::CHAP;
	_structureToStringTable[SID_COLONIST_LANDER] = constants::COLONIST_LANDER;
	_structureToStringTable[SID_COMMAND_CENTER] = constants::COMMAND_CENTER;
	_structureToStringTable[SID_COMMERCIAL] = constants::COMMERCIAL;
	_structureToStringTable[SID_FUSION_REACTOR] = constants::FUSION_REACTOR;
	_structureToStringTable[SID_HOT_LABORATORY] = constants::HOT_LABORATORY;
	_structureToStringTable[SID_LABORATORY] = constants::LABORATORY;
	_structureToStringTable[SID_MEDICAL_CENTER] = constants::MEDICAL_CENTER;
	_structureToStringTable[SID_MINE_FACILITY] = constants::MINE_FACILITY;
	_structureToStringTable[SID_MINE_SHAFT] = constants::MINE_SHAFT;
	_structureToStringTable[SID_PARK] = constants::PARK;
	_structureToStringTable[SID_SURFACE_POLICE] = constants::SURFACE_POLICE;
	_structureToStringTable[SID_UNDERGROUND_POLICE] = constants::UNDERGROUND_POLICE;
	_structureToStringTable[SID_RECREATION_CENTER] = constants::RECREATION_CENTER;
	_structureToStringTable[SID_RED_LIGHT_DISTRICT] = constants::RED_LIGHT_DISTRICT;
	_structureToStringTable[SID_RESIDENCE] = constants::RESIDENCE;
	_structureToStringTable[SID_ROBOT_COMMAND] = constants::ROBOT_COMMAND;
	_structureToStringTable[SID_SEED_FACTORY] = constants::SEED_FACTORY;
	_structureToStringTable[SID_SEED_LANDER] = constants::SEED_LANDER;
	_structureToStringTable[SID_SEED_POWER] = constants::SEED_POWER;
	_structureToStringTable[SID_SEED_SMELTER] = constants::SEED_SMELTER;
	_structureToStringTable[SID_SMELTER] = constants::SMELTER;
	_structureToStringTable[SID_SOLAR_PLANT] = constants::SOLAR_PLANT;
	_structureToStringTable[SID_STORAGE_TANKS] = constants::STORAGE_TANKS;
	_structureToStringTable[SID_SURFACE_FACTORY] = constants::SURFACE_FACTORY;
	_structureToStringTable[SID_UNDERGROUND_FACTORY] = constants::UNDERGROUND_FACTORY;
	_structureToStringTable[SID_UNIVERSITY] = constants::UNIVERSITY;
	_structureToStringTable[SID_WAREHOUSE] = constants::WAREHOUSE;

	_structureToStringTable[SID_TUBE] = constants::TUBE;
}
