#include "StructureTranslator.h"

#include "Constants.h"

std::map<std::string, StructureType> StructureTranslator::_stringToStructureTable;

StructureType StructureTranslator::translateFromString(const std::string& _s)
{
	if (_stringToStructureTable.empty())
		buildStringToStructureTable();

	if (_stringToStructureTable.find(_s) == _stringToStructureTable.end())
		return STRUCTURE_NONE;

	return _stringToStructureTable[_s];
}


void StructureTranslator::buildStringToStructureTable()
{
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
}