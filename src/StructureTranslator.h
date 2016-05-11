#pragma once

#include <string>
#include <map>

#include "Common.h"


/**
 * \class	StructureTranslator
 * \brief	Helper class to translate between structure name strings and enumerated ID values.
 * 
 * Honestly, translating from a string constant to a numeric ID is super inefficient. The other
 * way around makes way more sense. I want to refactor the code to eliminate the need for a
 * translation from a string to a numeric value (the other way around is perfectly reasonable.)
 */
class StructureTranslator
{
public:

	static StructureID translateFromString(const std::string& _s);
	static const std::string& translateToString(StructureID _s);

protected:

private:

	// Explicitly declared private to prevent instantiation.
	StructureTranslator() {}
	~StructureTranslator() {}

	static void buildTables();

	static std::map<std::string, StructureID>		_stringToStructureTable;
	static std::map<StructureID, std::string>		_structureToStringTable;
};