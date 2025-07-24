#pragma once

#include "ResearchFacility.h"

#include <libOPHD/EnumStructureID.h>


class Laboratory : public ResearchFacility
{
public:
	Laboratory(Tile& tile) :
		ResearchFacility{StructureID::Laboratory, tile}
	{
		maxScientistsAllowed(3);
		regularPointsPerScientist(1);
	}
};
