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
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(false);
		hasCrime(true);

		maxScientistsAllowed(3);
		regularPointsPerScientist(1);
	}


protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};
