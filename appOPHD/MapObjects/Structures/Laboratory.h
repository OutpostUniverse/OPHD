#pragma once

#include "ResearchFacility.h"

#include <libOPHD/EnumStructureID.h>

class Laboratory : public ResearchFacility
{
public:
    //TODO (cugone): Move to implementation file to avoid pulling in EnumStructureID for all includes
	Laboratory(Tile& tile) :
		ResearchFacility{StructureID::Laboratory, tile}
	{
		maxScientistsAllowed(3);
		regularPointsPerScientist(1);
	}
};
