#pragma once

#include "ResearchFacility.h"


class Laboratory : public ResearchFacility
{
public:
	Laboratory(Tile& tile) :
		ResearchFacility{StructureID::SID_LABORATORY, tile}
	{
		maxScientistsAllowed(3);
		regularPointsPerScientist(1);
	}
};
