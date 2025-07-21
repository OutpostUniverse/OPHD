#pragma once

#include "../Structure.h"


/**
 * Class for structures whose primary purpose is power production
 */
class PowerStructure : public Structure
{
public:
	PowerStructure(StructureID id, Tile& tile);
};
