#pragma once

#include <string>
#include <map>

#include "Common.h"

class RobotTranslator
{
public:

	static RobotType translateFromString(const std::string& _s);

protected:

private:

	// Explicitly declared private to prevent instantiation.
	RobotTranslator() {}
	~RobotTranslator() {}

	static void buildStringToRobotTable();

	static std::map<std::string, RobotType>		_stringToRobotTable;
};