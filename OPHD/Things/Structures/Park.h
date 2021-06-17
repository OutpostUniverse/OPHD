#pragma once

#include "Structure.h"

#include "../../Constants.h"


class Park : public Structure
{
public:
	Park() : Structure(constants::Park,
		"structures/park.sprite",
		StructureClass::Park,
		StructureID::SID_PARK)
	{
		maxAge(500);
		turnsToBuild(3);

		requiresCHAP(true);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(1);
	}
};
