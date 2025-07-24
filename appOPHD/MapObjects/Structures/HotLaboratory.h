#pragma once

#include "ResearchFacility.h"

#include <libOPHD/EnumStructureID.h>


class HotLaboratory : public ResearchFacility
{
public:
	HotLaboratory(Tile& tile) :
		ResearchFacility{StructureID::SID_HOT_LABORATORY, tile}
	{
		maxScientistsAllowed(3);
		hotPointsPerScientist(1.0f);
	}
};
