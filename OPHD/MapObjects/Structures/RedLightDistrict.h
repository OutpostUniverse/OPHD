#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class RedLightDistrict : public Structure
{
public:
	RedLightDistrict() : Structure(
		StructureClass::Residence,
		StructureID::SID_RED_LIGHT_DISTRICT)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};
