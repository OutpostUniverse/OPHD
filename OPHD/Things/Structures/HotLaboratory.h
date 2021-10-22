#pragma once

#include "Structure.h"

#include "../../Constants/Strings.h"


class HotLaboratory : public Structure
{
public:
	HotLaboratory() : Structure(constants::HotLaboratory,
		"structures/labo_surface.sprite",
		StructureClass::Laboratory,
		StructureID::SID_HOT_LABORATORY)
	{
		maxAge(500);
		turnsToBuild(5);
		integrityDecayRate(2);

		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(3);
	}
};
