#include "RobotTranslator.h"

#include "Constants.h"

std::map<std::string, RobotType> RobotTranslator::_stringToRobotTable;

RobotType RobotTranslator::translateFromString(const std::string& _s)
{
	if (_stringToRobotTable.empty())
		buildStringToRobotTable();

	if (_stringToRobotTable.find(_s) == _stringToRobotTable.end())
		return ROBOT_NONE;

	return _stringToRobotTable[_s];
}


void RobotTranslator::buildStringToRobotTable()
{
	_stringToRobotTable[constants::ROBODIGGER] = ROBOT_DIGGER;
	_stringToRobotTable[constants::ROBODOZER] = ROBOT_DOZER;
	_stringToRobotTable[constants::ROBOMINER] = ROBOT_MINER;
}