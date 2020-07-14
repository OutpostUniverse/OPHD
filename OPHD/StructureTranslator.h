#pragma once

#include "Common.h"

#include <array>
#include <string>
#include <map>


/**
 * \class	StructureTranslator
 * \brief	Helper class to translate between structure name strings and enumerated ID values.
 * 
 * Legacy class that's not really needed since the updates to using strictly structure ID's. I'm
 * keeping it intact as it's making reading from and writing to save game files way easier especially
 * as the structure ID's change during development.
 * 
 * \note	This class will remain during development and debugging periods only. After the structure
 *			list is finalized this class will be deprecated in favor of numerical structure ID's (SID_'s).
 * 
 */
class StructureTranslator
{
public:

	static StructureID translateFromString(const std::string& s);
	static const std::string& translateToString(StructureID s);

	static void init();

protected:

private:

	// Explicitly declared private to prevent instantiation.
	StructureTranslator() {}
	~StructureTranslator() {}

	static void buildTables();

	static std::map<std::string, StructureID> _stringToStructureTable;
	static std::array<std::string, StructureID::SID_COUNT> _structureToStringTable;
};
