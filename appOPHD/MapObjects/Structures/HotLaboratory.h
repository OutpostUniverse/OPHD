#pragma once

#include "ResearchFacility.h"

#include <libOPHD/EnumStructureID.h>


class HotLaboratory : public ResearchFacility
{
public:
	//TODO (cugone): Move to implementation file to avoid pulling in EnumStructureID for all includes
	HotLaboratory(Tile& tile) :
		ResearchFacility{StructureID::HotLaboratory, tile}
	{
		maxScientistsAllowed(3);
		hotPointsPerScientist(1.0f);
	}
};
