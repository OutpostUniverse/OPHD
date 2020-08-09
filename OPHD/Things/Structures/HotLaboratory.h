#pragma once

#include "Structure.h"

class HotLaboratory : public Structure
{
public:
	HotLaboratory() : Structure(constants::HOT_LABORATORY, "structures/labo_surface.sprite", StructureClass::Laboratory)
	{
		maxAge(500);
		turnsToBuild(5);

		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(3);
	}
};
