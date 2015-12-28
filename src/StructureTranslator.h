#pragma once

#include <string>
#include <map>

#include "Common.h"

class StructureTranslator
{
public:

	static StructureType translateFromString(const std::string& _s);

protected:

private:

	// Explicitly declared private to prevent instantiation.
	StructureTranslator() {}
	~StructureTranslator() {}

	static void buildStringToStructureTable();

	static std::map<std::string, StructureType>		_stringToStructureTable;
};