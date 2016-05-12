#include "StructureTranslator.h"

#include "Constants.h"

std::map<std::string, StructureID> StructureTranslator::_stringToStructureTable;
std::map<StructureID, std::string> StructureTranslator::_structureToStringTable;

StructureID StructureTranslator::translateFromString(const std::string& _s)
{
	if (_stringToStructureTable.empty())
		buildTables();

	if (_stringToStructureTable.find(_s) == _stringToStructureTable.end())
		return STRUCTURE_NONE;

	return _stringToStructureTable[_s];
}


const std::string& StructureTranslator::translateToString(StructureID _s)
{
	if (_structureToStringTable.empty())
		buildTables();

	if (_structureToStringTable.find(_s) == _structureToStringTable.end())
		return "";

	return _structureToStringTable[_s];	
}

void StructureTranslator::buildTables()
{
	// String to ID
	_stringToStructureTable[constants::AGRIDOME] = STRUCTURE_AGRIDOME;
	_stringToStructureTable[constants::AIR_SHAFT] = STRUCTURE_AIR_SHAFT;
	_stringToStructureTable[constants::CHAP] = STRUCTURE_CHAP;
	_stringToStructureTable[constants::COMMAND_CENTER] = STRUCTURE_COMMAND_CENTER;
	_stringToStructureTable[constants::MINE_FACILITY] = STRUCTURE_MINE_FACILITY;
	_stringToStructureTable[constants::SEED_FACTORY] = STRUCTURE_SEED_FACTORY;
	_stringToStructureTable[constants::SEED_LANDER] = STRUCTURE_SEED_LANDER;
	_stringToStructureTable[constants::SEED_POWER] = STRUCTURE_SEED_POWER;
	_stringToStructureTable[constants::SEED_SMELTER] = STRUCTURE_SEED_SMELTER;

	_stringToStructureTable[constants::AG_TUBE_INTERSECTION] = STRUCTURE_TUBE_INTERSECTION;
	_stringToStructureTable[constants::UG_TUBE_INTERSECTION] = STRUCTURE_TUBE_INTERSECTION;
	_stringToStructureTable[constants::AG_TUBE_RIGHT] = STRUCTURE_TUBE_RIGHT;
	_stringToStructureTable[constants::UG_TUBE_RIGHT] = STRUCTURE_TUBE_RIGHT;
	_stringToStructureTable[constants::AG_TUBE_LEFT] = STRUCTURE_TUBE_LEFT;
	_stringToStructureTable[constants::UG_TUBE_LEFT] = STRUCTURE_TUBE_LEFT;
	
	// ID To String
	_structureToStringTable[STRUCTURE_AGRIDOME] = constants::AGRIDOME;
	_structureToStringTable[STRUCTURE_AIR_SHAFT] = constants::AIR_SHAFT;
	_structureToStringTable[STRUCTURE_CHAP] = constants::CHAP;
	_structureToStringTable[STRUCTURE_COMMAND_CENTER] = constants::COMMAND_CENTER;
	_structureToStringTable[STRUCTURE_MINE_FACILITY] = constants::MINE_FACILITY;
	_structureToStringTable[STRUCTURE_SEED_FACTORY] = constants::SEED_FACTORY;
	_structureToStringTable[STRUCTURE_SEED_LANDER] = constants::SEED_LANDER;
	_structureToStringTable[STRUCTURE_SEED_POWER] = constants::SEED_POWER;
	_structureToStringTable[STRUCTURE_SEED_SMELTER] = constants::SEED_SMELTER;

	_structureToStringTable[STRUCTURE_TUBE_INTERSECTION] = constants::AG_TUBE_INTERSECTION;
	_structureToStringTable[STRUCTURE_TUBE_RIGHT] = constants::AG_TUBE_RIGHT;
	_structureToStringTable[STRUCTURE_TUBE_LEFT] = constants::AG_TUBE_LEFT;
}