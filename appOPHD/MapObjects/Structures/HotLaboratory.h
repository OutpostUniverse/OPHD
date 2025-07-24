#pragma once

#include "ResearchFacility.h"

#include <libOPHD/EnumStructureID.h>


class HotLaboratory : public ResearchFacility
{
public:
	HotLaboratory(Tile& tile) :
		ResearchFacility{StructureID::HotLaboratory, tile}
	{
		maxScientistsAllowed(3);
		hotPointsPerScientist(1.0f);
	}
};
