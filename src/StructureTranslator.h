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

	StructureTranslator() {} // Explicitly declared private.
	~StructureTranslator() {} // Explicitly declared private.

	static void buildTable();

	static std::map<std::string, StructureType>		_structureTable;
};