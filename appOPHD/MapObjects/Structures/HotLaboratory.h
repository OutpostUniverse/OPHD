#pragma once

#include "ResearchFacility.h"


class HotLaboratory : public ResearchFacility
{
public:
	HotLaboratory() :
		ResearchFacility{StructureClass::Laboratory, StructureID::SID_HOT_LABORATORY}
	{
		maxScientistsAllowed(3);
		hotPointsPerScientist(1.0f);
	}
};
