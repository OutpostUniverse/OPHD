#pragma once

#include "Structure.h"

#include "../../Constants.h"

class RedLightDistrict : public Structure
{
public:
	RedLightDistrict() : Structure(constants::RED_LIGHT_DISTRICT, "structures/red_light_district.sprite", StructureClass::Residence)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};
