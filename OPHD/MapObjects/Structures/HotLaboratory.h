#pragma once

#include "ResearchFacility.h"

#include "../../Constants/Strings.h"


class HotLaboratory : public ResearchFacility
{
public:
	HotLaboratory() :
		ResearchFacility(
		"structures/labo_surface.sprite",
		StructureClass::Laboratory,
		StructureID::SID_HOT_LABORATORY)
	{
		maxAge(500);
		turnsToBuild(5);
		integrityDecayRate(2);

		requiresCHAP(false);

		maxScientistsAllowed(3);
		hotPointsPerScientist(1.0f);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(3);
	}
};
