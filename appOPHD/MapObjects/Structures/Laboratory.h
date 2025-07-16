#pragma once

#include "ResearchFacility.h"


class Laboratory : public ResearchFacility
{
public:
	Laboratory(Tile& tile) :
		ResearchFacility{StructureClass::Laboratory, StructureID::SID_LABORATORY, tile}
	{
		maxScientistsAllowed(3);
		regularPointsPerScientist(1);
	}
};
