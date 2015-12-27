#include "StructureTranslator.h"

#include "Constants.h"

std::map<std::string, StructureType> StructureTranslator::_structureTable;

StructureType StructureTranslator::translateFromString(const std::string& _s)
{
	if (_structureTable.empty())
		buildTable();

	if (_structureTable.find(_s) == _structureTable.end())
		return STRUCTURE_NONE;

	return _structureTable[_s];
}


void StructureTranslator::buildTable()
{
	_structureTable[constants::AGRIDOME] = STRUCTURE_AGRIDOME;
	_structureTable[constants::AIR_SHAFT] = STRUCTURE_AIR_SHAFT;
	_structureTable[constants::CHAP] = STRUCTURE_CHAP;
	_structureTable[constants::COMMAND_CENTER] = STRUCTURE_COMMAND_CENTER;
	_structureTable[constants::MINE_FACILITY] = STRUCTURE_MINE_FACILITY;
	_structureTable[constants::SEED_FACTORY] = STRUCTURE_SEED_FACTORY;
	_structureTable[constants::SEED_LANDER] = STRUCTURE_SEED_LANDER;
	_structureTable[constants::SEED_POWER] = STRUCTURE_SEED_POWER;
	_structureTable[constants::SEED_SMELTER] = STRUCTURE_SEED_SMELTER;

	_structureTable[constants::AG_TUBE_INTERSECTION] = STRUCTURE_TUBE_INTERSECTION;
	_structureTable[constants::UG_TUBE_INTERSECTION] = STRUCTURE_TUBE_INTERSECTION;
	_structureTable[constants::AG_TUBE_RIGHT] = STRUCTURE_TUBE_RIGHT;
	_structureTable[constants::UG_TUBE_RIGHT] = STRUCTURE_TUBE_RIGHT;
	_structureTable[constants::AG_TUBE_LEFT] = STRUCTURE_TUBE_LEFT;
	_structureTable[constants::UG_TUBE_LEFT] = STRUCTURE_TUBE_LEFT;
}