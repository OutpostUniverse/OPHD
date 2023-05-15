#pragma once

#include "ResearchFacility.h"

#include "../../Constants/Strings.h"


class Laboratory : public ResearchFacility
{
public:
	Laboratory() : ResearchFacility(
		StructureClass::Laboratory,
		StructureID::SID_LABORATORY)
	{
		maxScientistsAllowed(3);
		regularPointsPerScientist(1);
	}
};
